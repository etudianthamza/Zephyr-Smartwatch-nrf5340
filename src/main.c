/*
 * Copyright (c) 2025 Smartwatch Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/input/input.h>
#include <zephyr/sys/util.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "All_sensors/sensors.h"
#include <math.h>
#include <stdlib.h>
#include "BLE/BLE_Configuration.h"
#include "Chronometer/chrono.h"
#include "RTC/rtc.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

// ==================== Configuration tactile ====================
#define NEXT_X       226
#define NEXT_Y       24
#define PREV_X       226
#define PREV_Y       315
#define TOLERANCE    20

static const struct device *const touch_dev =
    DEVICE_DT_GET(DT_NODELABEL(tsc2007_adafruit_2_8_tft_touch_v2));

static struct k_sem touch_sync;

// ==================== LED BLE ====================
#define BLE_LED DK_LED2
static struct k_timer ble_led_timer;

static void ble_led_timer_handler(struct k_timer *timer)
{
    dk_set_led_off(BLE_LED);
}

void ble_led_blink(void)
{
    dk_set_led_on(BLE_LED);
    k_timer_start(&ble_led_timer, K_MSEC(100), K_NO_WAIT);
}

// ==================== LED Chronomètre ====================
#define CHRONO_LED DK_LED1
static struct k_timer chrono_led_timer;

static void chrono_led_timer_handler(struct k_timer *timer)
{
    dk_set_led_off(CHRONO_LED);
}

static void chrono_led_blink(void)
{
    dk_set_led_on(CHRONO_LED);
    k_timer_start(&chrono_led_timer, K_MSEC(100), K_NO_WAIT);
}

// ==================== Gestion de veille ====================
static struct k_timer inactivity_timer;
static bool system_sleeping = false;
static bool need_sleep = false;
static struct k_sem wake_sem;
static struct k_sem ble_wake_sem;
static const struct device *display_dev;

static void reset_inactivity_timer(void)
{
    k_timer_stop(&inactivity_timer);
    k_timer_start(&inactivity_timer, K_SECONDS(60), K_NO_WAIT);
}

static void inactivity_timer_handler(struct k_timer *timer)
{
    need_sleep = true;
}

// ==================== Détection orientation pour réveil ====================
// Seuils à ajuster selon votre montre
#define ACCEL_X_MIN -0.13f
#define ACCEL_X_MAX  0.41f
#define ACCEL_Y_MIN -9.75f
#define ACCEL_Y_MAX  -7.76f
#define ACCEL_Z_MIN   2.38f
#define ACCEL_Z_MAX   4.38f

static bool is_watch_facing_user(void)
{
    float ax, ay, az;
    sensors_get_accel(&ax, &ay, &az);

    LOG_INF("Accel orientation check: X=%.2f, Y=%.2f, Z=%.2f", ax, ay, az);

    return (ax >= ACCEL_X_MIN && ax <= ACCEL_X_MAX &&
            ay >= ACCEL_Y_MIN && ay <= ACCEL_Y_MAX &&
            az >= ACCEL_Z_MIN && az <= ACCEL_Z_MAX);
}

static void enter_sleep(void)
{
    LOG_INF("Entering sleep mode (inactivity)");

    if (display_dev) {
        display_blanking_on(display_dev);
    }
    system_sleeping = true;
    need_sleep = false;

    while (system_sleeping) {
        int ret = k_sem_take(&wake_sem, K_SECONDS(1));
        if (ret == 0) {
            LOG_INF("Wake by button or touch");
            break;
        } else {
            sensors_update();           // température, humidité, pression
            sensors_update_motion();    // accéléromètre, gyroscope, magnétomètre
            if (is_watch_facing_user()) {
                LOG_INF("Wake by orientation (watch facing user)");
                break;
            }
        }
    }

    if (display_dev) {
        display_blanking_off(display_dev);
    }
    system_sleeping = false;
    LOG_INF("Woke up from sleep");

    k_sem_give(&ble_wake_sem);
    reset_inactivity_timer();
}

// Callback pour les boutons
static void button_changed(uint32_t button_state, uint32_t has_changed)
{
    k_sem_give(&wake_sem);
    if (!system_sleeping) {
        reset_inactivity_timer();
    }
}

// ==================== Gestion tactile ====================
static struct {
    size_t x;
    size_t y;
    bool pressed;
} touch_point;

static void touch_event_callback(struct input_event *evt, void *user_data)
{
    if (evt->code == INPUT_ABS_X) {
        touch_point.x = evt->value;
        LOG_INF("Touch X = %d", evt->value);
    }
    if (evt->code == INPUT_ABS_Y) {
        touch_point.y = evt->value;
        LOG_INF("Touch Y = %d", evt->value);
    }
    if (evt->code == INPUT_BTN_TOUCH) {
        touch_point.pressed = evt->value;
        LOG_INF("Touch pressed = %d", evt->value);
        if (evt->value) {
            if (system_sleeping) {
                k_sem_give(&wake_sem);
            } else {
                reset_inactivity_timer();
            }
        }
    }
    if (evt->sync) {
        LOG_INF("Touch sync received");
        k_sem_give(&touch_sync);
    }
}

INPUT_CALLBACK_DEFINE(touch_dev, touch_event_callback, NULL);

// ==================== Écrans et index ====================
static lv_obj_t *screens[4];
static int screen_index = 0;

// ==================== Timer Zephyr pour acquisition capteurs ====================
static struct k_timer acq_timer;
static volatile bool acq_flag = false;
static uint32_t epoch_s = 0;

static void timer_handler(struct k_timer *timer)
{
    epoch_s++;
    acq_flag = true;
}

static void timer_init(void)
{
    k_timer_init(&acq_timer, timer_handler, NULL);
    k_timer_start(&acq_timer, K_SECONDS(1), K_SECONDS(1));
    LOG_INF("Timer démarré : période 1s");
}

// ==================== Mise à jour des capteurs sur l'interface ====================
static void update_ui_sensors(void)
{
    float temp = sensors_get_temperature();
    float hum  = sensors_get_humidity();
    float press = sensors_get_pressure();
    float mx, my, mz;
    sensors_get_magn(&mx, &my, &mz);
    float ax, ay, az;
    sensors_get_accel(&ax, &ay, &az);
    float gx, gy, gz;
    sensors_get_gyro(&gx, &gy, &gz);

    rtc_time_t now;
    int rtc_ok = rtc_get_time(&now);
    if (rtc_ok == 0) {
        LOG_INF("=== Capteurs (t=%u s, %02d:%02d:%02d) ===",
                epoch_s, now.hours, now.minutes, now.seconds);
    } else {
        LOG_INF("=== Capteurs (t=%u s, RTC indisponible) ===", epoch_s);
    }

    LOG_INF("Température : %.1f °C", (double)temp);
    LOG_INF("Humidité    : %.1f %%", (double)hum);
    LOG_INF("Pression    : %.1f hPa", (double)press);
    LOG_INF("Magnétomètre: X=%.2f  Y=%.2f  Z=%.2f", (double)mx, (double)my, (double)mz);
    LOG_INF("Accéléromètre: X=%.2f m/s²  Y=%.2f m/s²  Z=%.2f m/s²", (double)ax, (double)ay, (double)az);
    LOG_INF("Gyroscope    : X=%.2f rad/s  Y=%.2f rad/s  Z=%.2f rad/s", (double)gx, (double)gy, (double)gz);

    char buf[32];

    snprintf(buf, sizeof(buf), "%.1f°C", (double)temp);
    if (ui_DegreCeluis) lv_label_set_text(ui_DegreCeluis, buf);
    if (ui_DegreCeluis2) lv_label_set_text(ui_DegreCeluis2, buf);

    snprintf(buf, sizeof(buf), "%.1f%%", (double)hum);
    if (ui_Label9) lv_label_set_text(ui_Label9, buf);

    snprintf(buf, sizeof(buf), "%.1f hPa", (double)press);
    if (ui_Pressure) lv_label_set_text(ui_Pressure, buf);

    float heading = atan2f(-my, mx) * 180.0f / 3.14159f;
    if (heading < 0) heading += 360.0f;
    snprintf(buf, sizeof(buf), "%.0f °", (double)heading);
    if (ui_degreDirection) lv_label_set_text(ui_degreDirection, buf);

    const char *cardinal = sensors_heading_to_cardinal(heading);
    if (ui_direction) lv_label_set_text(ui_direction, cardinal);

    snprintf(buf, sizeof(buf), "%.2f", (double)ax);
    if (ui_valueOfX) lv_label_set_text(ui_valueOfX, buf);
    snprintf(buf, sizeof(buf), "%.2f", (double)ay);
    if (ui_valueOfY) lv_label_set_text(ui_valueOfY, buf);
    snprintf(buf, sizeof(buf), "%.2f", (double)az);
    if (ui_valueOfZ) lv_label_set_text(ui_valueOfZ, buf);

    snprintf(buf, sizeof(buf), "%.2f", (double)gx);
    if (ui_valueOfX2) lv_label_set_text(ui_valueOfX2, buf);
    snprintf(buf, sizeof(buf), "%.2f", (double)gy);
    if (ui_valueOfY2) lv_label_set_text(ui_valueOfY2, buf);
    snprintf(buf, sizeof(buf), "%.2f", (double)gz);
    if (ui_valueOfZ2) lv_label_set_text(ui_valueOfZ2, buf);
}

/* ==================== Tâche BLE ==================== */
#define BLE_STACK_SIZE 1024
K_THREAD_STACK_DEFINE(ble_stack_area, BLE_STACK_SIZE);
static struct k_thread ble_thread;

static void ble_thread_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        if (system_sleeping) {
            k_sem_take(&ble_wake_sem, K_FOREVER);
        }

        k_sleep(K_SECONDS(2));

        if (system_sleeping) {
            continue;
        }

        float temp = sensors_get_temperature();
        float hum  = sensors_get_humidity();

        struct ble_env_payload payload;
        payload.epoch_s = (uint32_t)(k_uptime_get() / 1000);
        payload.temp_cC = (int16_t)(temp * 100.0f);
        payload.hum_cpc = (uint16_t)(hum * 100.0f);

        ble_led_blink();
        ble_send_env_payload(&payload);
        ble_update_advertising(&payload);
    }
}

// ==================== Fonctions de rappel pour les boutons du chronomètre ====================
static void chrono_start_clicked(lv_event_t *e)
{
    chrono_start();
}

static void chrono_stop_clicked(lv_event_t *e)
{
    chrono_stop();
}

static void chrono_reset_clicked(lv_event_t *e)
{
    chrono_reset();
}

// ==================== Mise à jour LVGL de l'affichage du chronomètre ====================
static void chrono_update_display(lv_timer_t *timer)
{
    int h, m, s;
    chrono_get_time(&h, &m, &s);

    char buf[3];
    snprintf(buf, sizeof(buf), "%02d", h);
    if (ui_LabelHH) lv_label_set_text(ui_LabelHH, buf);
    else LOG_ERR("chrono_update: ui_LabelHH NULL");

    snprintf(buf, sizeof(buf), "%02d", m);
    if (ui_LabelMM) lv_label_set_text(ui_LabelMM, buf);
    else LOG_ERR("chrono_update: ui_LabelMM NULL");

    snprintf(buf, sizeof(buf), "%02d", s);
    if (ui_LabelSS) lv_label_set_text(ui_LabelSS, buf);
    else LOG_ERR("chrono_update: ui_LabelSS NULL");
}

// ==================== Mise à jour LVGL de l'heure / date ====================
static bool rtc_available = false;

static void update_rtc_display(lv_timer_t *timer)
{
    if (!rtc_available) return;

    rtc_time_t now;

    if (rtc_get_time(&now) != 0) {
        LOG_ERR("Impossible de lire le RTC");
        return;
    }

    char buf[3];

    // ----- Écran Home -----
    snprintf(buf, sizeof(buf), "%02d", now.hours);
    if (ui_LabelHour) lv_label_set_text(ui_LabelHour, buf);
    snprintf(buf, sizeof(buf), "%02d", now.minutes);
    if (ui_LabelMinute) lv_label_set_text(ui_LabelMinute, buf);
    snprintf(buf, sizeof(buf), "%02d", now.days);
    if (ui_LabelDay) lv_label_set_text(ui_LabelDay, buf);
    snprintf(buf, sizeof(buf), "%02d", now.months);
    if (ui_LabelMonth) lv_label_set_text(ui_LabelMonth, buf);
    snprintf(buf, sizeof(buf), "%02d", now.years);
    if (ui_LabelYear) lv_label_set_text(ui_LabelYear, buf);

    // ----- Écran Weather -----
    snprintf(buf, sizeof(buf), "%02d", now.hours);
    if (ui_LabelHour2) lv_label_set_text(ui_LabelHour2, buf);
    snprintf(buf, sizeof(buf), "%02d", now.minutes);
    if (ui_LabelMinute2) lv_label_set_text(ui_LabelMinute2, buf);
    snprintf(buf, sizeof(buf), "%02d", now.days);
    if (ui_LabelDay2) lv_label_set_text(ui_LabelDay2, buf);
    snprintf(buf, sizeof(buf), "%02d", now.months);
    if (ui_LabelMonth2) lv_label_set_text(ui_LabelMonth2, buf);
    snprintf(buf, sizeof(buf), "%02d", now.years);
    if (ui_LabelYear2) lv_label_set_text(ui_LabelYear2, buf);

    // ----- Écran Direction / Accél/Gyro -----
    snprintf(buf, sizeof(buf), "%02d", now.hours);
    if (ui_LabelHour3) lv_label_set_text(ui_LabelHour3, buf);
    snprintf(buf, sizeof(buf), "%02d", now.minutes);
    if (ui_LabelMinute3) lv_label_set_text(ui_LabelMinute3, buf);
    snprintf(buf, sizeof(buf), "%02d", now.days);
    if (ui_LabelDay3) lv_label_set_text(ui_LabelDay3, buf);
    snprintf(buf, sizeof(buf), "%02d", now.months);
    if (ui_LabelMonth3) lv_label_set_text(ui_LabelMonth3, buf);
    snprintf(buf, sizeof(buf), "%02d", now.years);
    if (ui_LabelYear3) lv_label_set_text(ui_LabelYear3, buf);
}

// ==================== Programme principal ====================
int main(void)
{
    // ---- Initialisation de l'affichage ----
    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display device not ready");
        return 0;
    }
    display_blanking_off(display_dev);

    ui_init();
    screens[0] = ui_HomePage;
    screens[1] = ui_Weather;
    screens[2] = ui_DirectionMaAxAndGy;
    screens[3] = ui_Chronometre;

    if (sensors_init() != 0) {
        LOG_ERR("Sensor initialization failed (continuing anyway)");
    }
    
    // ---- Initialisation des LEDs ----
    if (dk_leds_init() != 0) {
        LOG_ERR("LEDs init failed");
    } else {
        LOG_INF("LEDs initialized");
        // Test des deux LEDs
        dk_set_led_on(DK_LED1);
        dk_set_led_on(DK_LED2);
        k_sleep(K_MSEC(500));
        dk_set_led_off(DK_LED1);
        dk_set_led_off(DK_LED2);
        LOG_INF("LED1 and LED2 tested");
    }

    // ---- Timer pour la LED BLE ----
    k_timer_init(&ble_led_timer, ble_led_timer_handler, NULL);

    // ---- Timer pour la LED Chronomètre ----
    k_timer_init(&chrono_led_timer, chrono_led_timer_handler, NULL);
    chrono_set_tick_callback(chrono_led_blink);

    // ---- Gestion de veille ----
    k_sem_init(&wake_sem, 0, 1);
    k_sem_init(&ble_wake_sem, 0, 1);
    k_timer_init(&inactivity_timer, inactivity_timer_handler, NULL);
    reset_inactivity_timer();

    // ---- Initialisation des boutons ----
    if (dk_buttons_init(button_changed) != 0) {
        LOG_ERR("Button init failed");
    } else {
        LOG_INF("Buttons initialized");
    }

    // ---- Initialisation du chronomètre ----
    chrono_init();

    // ---- Initialisation du RTC ----
    if (rtc_init() == 0) {
        rtc_available = true;
        LOG_INF("RTC initialisé");
        rtc_time_t initial_time = {
            .seconds = 0,
            .minutes = 15,
            .hours   = 10,
            .days    = 3,
            .weekday = 3,
            .months  = 4,
            .years   = 26
        };
        if (rtc_set_time(&initial_time) == 0) {
            LOG_INF("Heure réglée : %02d/%02d/20%02d %02d:%02d:%02d",
                    initial_time.days, initial_time.months, initial_time.years,
                    initial_time.hours, initial_time.minutes, initial_time.seconds);
        } else {
            LOG_WRN("Échec du réglage initial de l'heure");
        }
    } else {
        LOG_WRN("RTC non disponible, horloge non affichée");
    }

    // ---- Initialisation du tactile ----
    if (!device_is_ready(touch_dev)) {
        LOG_ERR("Touch device %s not ready", touch_dev->name);
    } else {
        LOG_INF("Touch device %s ready", touch_dev->name);
    }
    k_sem_init(&touch_sync, 0, 1);

    // ---- Initialisation du timer Zephyr pour l'acquisition ----
    timer_init();

    // ---- Initialisation BLE ----
    if (ble_init() != 0) {
        LOG_ERR("BLE initialization failed");
    } else {
        LOG_INF("BLE initialized");
    }

    // ---- Création du thread BLE ----
    k_thread_create(&ble_thread, ble_stack_area, BLE_STACK_SIZE,
                    ble_thread_fn, NULL, NULL, NULL,
                    4, 0, K_NO_WAIT);
    k_thread_name_set(&ble_thread, "ble");

    // ---- Timers LVGL pour le chronomètre et l'heure ----
    lv_timer_t *chrono_timer = lv_timer_create(chrono_update_display, 1000, NULL);
    lv_timer_ready(chrono_timer);

    if (rtc_available) {
        lv_timer_t *rtc_timer = lv_timer_create(update_rtc_display, 1000, NULL);
        lv_timer_ready(rtc_timer);
    }

    // ---- Événements des boutons du chronomètre ----
    if (ui_ButtonStart) lv_obj_add_event_cb(ui_ButtonStart, chrono_start_clicked, LV_EVENT_CLICKED, NULL);
    if (ui_ButtonStop)  lv_obj_add_event_cb(ui_ButtonStop,  chrono_stop_clicked,  LV_EVENT_CLICKED, NULL);
    if (ui_ButtonReset) lv_obj_add_event_cb(ui_ButtonReset, chrono_reset_clicked, LV_EVENT_CLICKED, NULL);

    LOG_INF("Entering main loop");

    while (1) {
        if (need_sleep && !system_sleeping) {
            enter_sleep();
        }

        if (acq_flag && !system_sleeping) {
            acq_flag = false;
            sensors_update();           // environmental
            sensors_update_motion();    // motion (accel, gyro, magn);
            update_ui_sensors();
        }

        lv_timer_handler();

        if (k_sem_take(&touch_sync, K_NO_WAIT) == 0) {
            size_t x = touch_point.x;
            size_t y = touch_point.y;
            bool pressed = touch_point.pressed;

            LOG_INF("Touch event: pressed=%d, x=%d, y=%d", pressed, x, y);

            if (pressed) {
                if (abs((int)x - NEXT_X) < TOLERANCE && abs((int)y - NEXT_Y) < TOLERANCE) {
                    LOG_INF("NEXT zone detected -> switching to next screen");
                    screen_index = (screen_index + 1) % 4;
                    lv_scr_load_anim(screens[screen_index], LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                }
                else if (abs((int)x - PREV_X) < TOLERANCE && abs((int)y - PREV_Y) < TOLERANCE) {
                    LOG_INF("PREV zone detected -> switching to previous screen");
                    screen_index = (screen_index - 1 + 4) % 4;
                    lv_scr_load_anim(screens[screen_index], LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
                } else {
                    LOG_INF("Touch at (%d,%d) does not match any zone", x, y);
                }
            }
        }

        k_sleep(K_MSEC(10));
    }

    return 0;
}