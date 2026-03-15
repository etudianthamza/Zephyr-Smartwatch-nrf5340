/*
 * Copyright (c) 2025 Smartwatch Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/input/input.h>
#include <zephyr/sys/util.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "sensors.h"
#include <math.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

// ==================== Configuration tactile ====================
// Coordonnées supposées (à ajuster après observation des logs)
#define NEXT_X       226
#define NEXT_Y       24
#define PREV_X       226
#define PREV_Y       315
#define TOLERANCE    20   // Augmenté pour couvrir une plus grande zone

static const struct device *const touch_dev =
    DEVICE_DT_GET(DT_NODELABEL(tsc2007_adafruit_2_8_tft_touch_v2));

static struct k_sem touch_sync;

static struct {
    size_t x;
    size_t y;
    bool pressed;
} touch_point;

static void touch_event_callback(struct input_event *evt, void *user_data)
{
    if (evt->code == INPUT_ABS_X) {
        touch_point.x = evt->value;
        LOG_DBG("X = %d", evt->value);  // Log debug
    }
    if (evt->code == INPUT_ABS_Y) {
        touch_point.y = evt->value;
        LOG_DBG("Y = %d", evt->value);
    }
    if (evt->code == INPUT_BTN_TOUCH) {
        touch_point.pressed = evt->value;
        LOG_DBG("Pressed = %d", evt->value);
    }
    if (evt->sync) {
        LOG_DBG("Sync received");
        k_sem_give(&touch_sync);
    }
}

INPUT_CALLBACK_DEFINE(touch_dev, touch_event_callback, NULL);

// ==================== Écrans et index ====================
static lv_obj_t *screens[4];
static int screen_index = 0;

// ==================== Mise à jour des capteurs ====================
static void sensor_update_cb(lv_timer_t *timer)
{
    sensors_update();

    float temp = sensors_get_temperature();
    float hum  = sensors_get_humidity();
    float press = sensors_get_pressure();
    float mx, my, mz;
    sensors_get_magn(&mx, &my, &mz);

    char buf[16];

    snprintf(buf, sizeof(buf), "%.1f°C", (double)temp);
    lv_label_set_text(ui_DegreCeluis, buf);
    lv_label_set_text(ui_DegreCeluis2, buf);

    snprintf(buf, sizeof(buf), "%.1f%%", (double)hum);
    lv_label_set_text(ui_Label9, buf);

    snprintf(buf, sizeof(buf), "%.1f kPa", (double)press);
    lv_label_set_text(ui_Pressure, buf);

    float heading = atan2f(my, mx) * 180.0f / 3.14159f;
    if (heading < 0) heading += 360.0f;
    snprintf(buf, sizeof(buf), "%.0f °", (double)heading);
    lv_label_set_text(ui_degreDirection, buf);

    const char *cardinal;
    if (heading >= 337.5f || heading < 22.5f)      cardinal = "N";
    else if (heading < 67.5f)                      cardinal = "NE";
    else if (heading < 112.5f)                     cardinal = "E";
    else if (heading < 157.5f)                     cardinal = "SE";
    else if (heading < 202.5f)                     cardinal = "S";
    else if (heading < 247.5f)                     cardinal = "SO";
    else if (heading < 292.5f)                     cardinal = "O";
    else                                           cardinal = "NO";
    lv_label_set_text(ui_direction, cardinal);
}

// ==================== Programme principal ====================
int main(void)
{
    // ---- Initialisation de l'affichage ----
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
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

    // ---- Initialisation du tactile ----
    if (!device_is_ready(touch_dev)) {
        LOG_ERR("Touch device %s not ready", touch_dev->name);
    } else {
        LOG_INF("Touch device %s ready", touch_dev->name);
    }
    k_sem_init(&touch_sync, 0, 1);

    lv_timer_create(sensor_update_cb, 2000, NULL);   // capteurs toutes les 2s

    LOG_INF("Entering main loop");

    while (1) {
        lv_timer_handler();

        // Vérification non bloquante du sémaphore tactile
        if (k_sem_take(&touch_sync, K_NO_WAIT) == 0) {
            // Copie des données sous protection implicite (pas de concurrence car callback fini)
            size_t x = touch_point.x;
            size_t y = touch_point.y;
            bool pressed = touch_point.pressed;

            LOG_INF("Touch event: pressed=%d, x=%d, y=%d", pressed, x, y);

            if (pressed) {
                // Vérification des zones
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