#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "sensors.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

/* Array of screen pointers – will be filled after ui_init() */
static lv_obj_t *screens[4];
static int screen_index = 0;

/* Timer callback: switch to next screen with animation */
static void screen_switch_cb(lv_timer_t *timer)
{
    screen_index = (screen_index + 1) % 4;
    lv_scr_load_anim(screens[screen_index], LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
}

/* Timer callback: update sensor data and refresh UI */
static void sensor_update_cb(lv_timer_t *timer)
{
    sensors_update();

    // Récupérer les valeurs
    float temp = sensors_get_temperature();
    float hum = sensors_get_humidity();
    float press = sensors_get_pressure();
    float mx, my, mz;
    sensors_get_magn(&mx, &my, &mz);

    // Mise à jour des labels (les variables globales sont déclarées dans les headers)
    char buf[16];

    // Température sur HomePage et Weather
    snprintf(buf, sizeof(buf), "%.1f°C", temp);
    lv_label_set_text(ui_DegreCeluis, buf);
    lv_label_set_text(ui_DegreCeluis2, buf);

    // Humidité sur Weather
    snprintf(buf, sizeof(buf), "%.1f%%", hum);
    lv_label_set_text(ui_Label9, buf);

    // Pression sur Weather
    snprintf(buf, sizeof(buf), "%.1f kPa", press); // ou hPa si vous préférez
    lv_label_set_text(ui_Pressure, buf);

    // Direction (cap magnétique simple)
    // Calcul de l'angle en degrés à partir de mx, my (plan horizontal)
    float heading = atan2f(my, mx) * 180.0f / 3.14159f;
    if (heading < 0) heading += 360.0f;

    snprintf(buf, sizeof(buf), "%.0f °", heading);
    lv_label_set_text(ui_degreDirection, buf);

    // Direction cardinale
    const char *cardinal;
    if (heading >= 337.5 || heading < 22.5) cardinal = "N";
    else if (heading < 67.5) cardinal = "NE";
    else if (heading < 112.5) cardinal = "E";
    else if (heading < 157.5) cardinal = "SE";
    else if (heading < 202.5) cardinal = "S";
    else if (heading < 247.5) cardinal = "SO";
    else if (heading < 292.5) cardinal = "O";
    else cardinal = "NO";
    lv_label_set_text(ui_direction, cardinal);

    // (Optionnel) Mise à jour de l'accélération si vous avez un écran dédié
    // float ax, ay, az; sensors_get_accel(&ax, &ay, &az);
    // ...
}

int main(void)
{
    const struct device *display_dev;

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
if (!device_is_ready(display_dev)) {
    printk("Display device not ready\n");
    return 0;
}
printk("Display %s is ready\n", display_dev->name);
    display_blanking_off(display_dev);

    /* Initialize UI */
    ui_init();

    /* Populate screen array */
    screens[0] = ui_HomePage;
    screens[1] = ui_Weather;
    screens[2] = ui_DirectionMaAxAndGy;
    screens[3] = ui_Chronometre;

    /* Initialize sensors */
    if (sensors_init() != 0) {
        LOG_ERR("Sensor initialization failed");
        // On peut continuer quand même, mais les valeurs resteront à zéro
    }

    /* Create timers */
    lv_timer_create(screen_switch_cb, 5000, NULL);   // changement d'écran toutes les 5s
    lv_timer_create(sensor_update_cb, 2000, NULL);   // mise à jour capteurs toutes les 2s

    /* Main loop */
    while (1) {
        lv_timer_handler();
        k_sleep(K_MSEC(10));
    }
    return 0;
}