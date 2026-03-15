#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <math.h>

LOG_MODULE_REGISTER(sensors, LOG_LEVEL_INF);

static const struct device *hts221;
static const struct device *lis3mdl;
static const struct device *lps25hb;
static const struct device *lsm6ds0;

static struct sensor_value temp_val, hum_val, press_val;
static struct sensor_value magn_xyz[3];
static struct sensor_value accel_xyz[3];

static void scan_i2c_bus(const struct device *i2c_dev)
{
    if (!i2c_dev) {
        LOG_ERR("I2C device is NULL, cannot scan");
        return;
    }

    LOG_INF("Scanning I2C bus %s...", i2c_dev->name);
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        struct i2c_msg msg = {
            .buf = NULL,
            .len = 0,
            .flags = I2C_MSG_WRITE | I2C_MSG_STOP
        };
        int ret = i2c_transfer(i2c_dev, &msg, 1, addr);
        if (ret == 0) {
            LOG_INF("Found device at 0x%02X", addr);
        }
    }
}

int sensors_init(void)
{
    int ret;

    hts221 = DEVICE_DT_GET(DT_NODELABEL(hts221));
    lis3mdl = DEVICE_DT_GET(DT_NODELABEL(lis3mdl));
    lps25hb = DEVICE_DT_GET(DT_NODELABEL(lps25hb));
    lsm6ds0 = DEVICE_DT_GET(DT_NODELABEL(lsm6ds0));

    LOG_INF("HTS221 device pointer: %p", hts221);
    LOG_INF("LIS3MDL device pointer: %p", lis3mdl);
    LOG_INF("LPS25HB device pointer: %p", lps25hb);
    LOG_INF("LSM6DS0 device pointer: %p", lsm6ds0);

    if (!device_is_ready(hts221)) {
        LOG_ERR("HTS221 not ready");
    } else {
        LOG_INF("HTS221 is ready");
    }
    if (!device_is_ready(lis3mdl)) {
        LOG_ERR("LIS3MDL not ready");
    } else {
        LOG_INF("LIS3MDL is ready");
    }
    if (!device_is_ready(lps25hb)) {
        LOG_ERR("LPS25HB not ready");
    } else {
        LOG_INF("LPS25HB is ready");
    }
    if (!device_is_ready(lsm6ds0)) {
        LOG_ERR("LSM6DS0 not ready");
    } else {
        LOG_INF("LSM6DS0 is ready");
    }

    // Récupérer le bus I2C à partir du premier capteur prêt
    const struct device *i2c_bus = NULL;
    if (device_is_ready(hts221)) {
        i2c_bus = DEVICE_DT_GET(DT_BUS(DT_NODELABEL(hts221)));
    } else if (device_is_ready(lis3mdl)) {
        i2c_bus = DEVICE_DT_GET(DT_BUS(DT_NODELABEL(lis3mdl)));
    } else if (device_is_ready(lps25hb)) {
        i2c_bus = DEVICE_DT_GET(DT_BUS(DT_NODELABEL(lps25hb)));
    } else if (device_is_ready(lsm6ds0)) {
        i2c_bus = DEVICE_DT_GET(DT_BUS(DT_NODELABEL(lsm6ds0)));
    }

    if (i2c_bus && device_is_ready(i2c_bus)) {
        scan_i2c_bus(i2c_bus);
    } else {
        LOG_ERR("No I2C bus ready for scanning");
    }

    // Test de lecture pour chaque capteur
    if (device_is_ready(hts221)) {
        ret = sensor_sample_fetch(hts221);
        if (ret == 0) {
            sensor_channel_get(hts221, SENSOR_CHAN_AMBIENT_TEMP, &temp_val);
            sensor_channel_get(hts221, SENSOR_CHAN_HUMIDITY, &hum_val);
            LOG_INF("Test HTS221: temp=%d.%d, hum=%d.%d",
                    temp_val.val1, temp_val.val2, hum_val.val1, hum_val.val2);
        } else {
            LOG_ERR("Test HTS221 fetch failed: %d", ret);
        }
    }

    if (device_is_ready(lps25hb)) {
        ret = sensor_sample_fetch(lps25hb);
        if (ret == 0) {
            sensor_channel_get(lps25hb, SENSOR_CHAN_PRESS, &press_val);
            LOG_INF("Test LPS25HB: press=%d.%d", press_val.val1, press_val.val2);
        } else {
            LOG_ERR("Test LPS25HB fetch failed: %d", ret);
        }
    }

    if (device_is_ready(lis3mdl)) {
        ret = sensor_sample_fetch(lis3mdl);
        if (ret == 0) {
            sensor_channel_get(lis3mdl, SENSOR_CHAN_MAGN_XYZ, magn_xyz);
            LOG_INF("Test LIS3MDL: mag x=%d.%d, y=%d.%d, z=%d.%d",
                    magn_xyz[0].val1, magn_xyz[0].val2,
                    magn_xyz[1].val1, magn_xyz[1].val2,
                    magn_xyz[2].val1, magn_xyz[2].val2);
        } else {
            LOG_ERR("Test LIS3MDL fetch failed: %d", ret);
        }
    }

    if (device_is_ready(lsm6ds0)) {
        ret = sensor_sample_fetch(lsm6ds0);
        if (ret == 0) {
            sensor_channel_get(lsm6ds0, SENSOR_CHAN_ACCEL_XYZ, accel_xyz);
            LOG_INF("Test LSM6DS0: acc x=%d.%d, y=%d.%d, z=%d.%d",
                    accel_xyz[0].val1, accel_xyz[0].val2,
                    accel_xyz[1].val1, accel_xyz[1].val2,
                    accel_xyz[2].val1, accel_xyz[2].val2);
        } else {
            LOG_ERR("Test LSM6DS0 fetch failed: %d", ret);
        }
    }

    return 0;
}

void sensors_update(void)
{
    int ret;

    if (device_is_ready(hts221)) {
        ret = sensor_sample_fetch(hts221);
        if (ret < 0) {
            LOG_ERR("HTS221 fetch failed: %d", ret);
        } else {
            sensor_channel_get(hts221, SENSOR_CHAN_AMBIENT_TEMP, &temp_val);
            sensor_channel_get(hts221, SENSOR_CHAN_HUMIDITY, &hum_val);
        }
    }

    if (device_is_ready(lps25hb)) {
        ret = sensor_sample_fetch(lps25hb);
        if (ret < 0) {
            LOG_ERR("LPS25HB fetch failed: %d", ret);
        } else {
            sensor_channel_get(lps25hb, SENSOR_CHAN_PRESS, &press_val);
        }
    }

    if (device_is_ready(lis3mdl)) {
        ret = sensor_sample_fetch(lis3mdl);
        if (ret < 0) {
            LOG_ERR("LIS3MDL fetch failed: %d", ret);
        } else {
            sensor_channel_get(lis3mdl, SENSOR_CHAN_MAGN_XYZ, magn_xyz);
        }
    }

    if (device_is_ready(lsm6ds0)) {
        ret = sensor_sample_fetch(lsm6ds0);
        if (ret < 0) {
            LOG_ERR("LSM6DS0 fetch failed: %d", ret);
        } else {
            sensor_channel_get(lsm6ds0, SENSOR_CHAN_ACCEL_XYZ, accel_xyz);
        }
    }
}

float sensors_get_temperature(void)
{
    return sensor_value_to_double(&temp_val);
}

float sensors_get_humidity(void)
{
    return sensor_value_to_double(&hum_val);
}

float sensors_get_pressure(void)
{
    return sensor_value_to_double(&press_val);
}

void sensors_get_magn(float *x, float *y, float *z)
{
    *x = sensor_value_to_double(&magn_xyz[0]);
    *y = sensor_value_to_double(&magn_xyz[1]);
    *z = sensor_value_to_double(&magn_xyz[2]);
}

void sensors_get_accel(float *x, float *y, float *z)
{
    *x = sensor_value_to_double(&accel_xyz[0]);
    *y = sensor_value_to_double(&accel_xyz[1]);
    *z = sensor_value_to_double(&accel_xyz[2]);
}