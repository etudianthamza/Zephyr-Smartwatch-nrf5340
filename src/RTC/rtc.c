#include "rtc.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(rtc, LOG_LEVEL_DBG);

/* Adresse I2C du RV‑8263‑C8 */
#define RV8263_I2C_ADDR     0x51

/* Registres internes */
#define RV8263_REG_CTRL1    0x00
#define RV8263_REG_CTRL2    0x01
#define RV8263_REG_SECONDS  0x04
#define RV8263_REG_MINUTES  0x05
#define RV8263_REG_HOURS    0x06
#define RV8263_REG_DAYS     0x07
#define RV8263_REG_WEEKDAY  0x08
#define RV8263_REG_MONTHS   0x09
#define RV8263_REG_YEARS    0x0A

/* Conversion BCD <-> décimal */
static inline uint8_t bcd_to_dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static inline uint8_t dec_to_bcd(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

/* Écriture d’un seul registre */
static int rv8263_write_reg(const struct device *i2c_dev, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    return i2c_write(i2c_dev, buf, sizeof(buf), RV8263_I2C_ADDR);
}

/* Lecture en rafale de plusieurs registres */
static int rv8263_read_regs(const struct device *i2c_dev, uint8_t reg,
                            uint8_t *data, uint8_t len)
{
    return i2c_write_read(i2c_dev, RV8263_I2C_ADDR, &reg, 1, data, len);
}

/* Initialisation du RTC (mode 24h, désactive interruptions) */
static int rv8263_init(const struct device *i2c_dev)
{
    int ret;

    ret = rv8263_write_reg(i2c_dev, RV8263_REG_CTRL1, 0x00);
    if (ret < 0) {
        LOG_ERR("Erreur écriture CTRL1: %d", ret);
        return ret;
    }

    ret = rv8263_write_reg(i2c_dev, RV8263_REG_CTRL2, 0x00);
    if (ret < 0) {
        LOG_ERR("Erreur écriture CTRL2: %d", ret);
        return ret;
    }

    LOG_INF("RV‑8263‑C8 initialisé");
    return 0;
}

/* Écriture de l’heure complète */
static int rv8263_set_time_raw(const struct device *i2c_dev, const rtc_time_t *t)
{
    uint8_t buf[8];

    buf[0] = RV8263_REG_SECONDS;
    buf[1] = dec_to_bcd(t->seconds);
    buf[2] = dec_to_bcd(t->minutes);
    buf[3] = dec_to_bcd(t->hours);
    buf[4] = dec_to_bcd(t->days);
    buf[5] = dec_to_bcd(t->weekday);
    buf[6] = dec_to_bcd(t->months);
    buf[7] = dec_to_bcd(t->years);

    int ret = i2c_write(i2c_dev, buf, sizeof(buf), RV8263_I2C_ADDR);
    if (ret < 0) {
        LOG_ERR("Erreur set_time_raw: %d", ret);
    }
    return ret;
}

/* Lecture de l’heure complète */
static int rv8263_get_time_raw(const struct device *i2c_dev, rtc_time_t *t)
{
    uint8_t raw[7];

    int ret = rv8263_read_regs(i2c_dev, RV8263_REG_SECONDS, raw, 7);
    if (ret < 0) {
        LOG_ERR("Erreur get_time_raw: %d", ret);
        return ret;
    }

    /* Bit 7 du registre SECONDS = VL (Voltage Low) – on le masque */
    t->seconds = bcd_to_dec(raw[0] & 0x7F);
    t->minutes = bcd_to_dec(raw[1] & 0x7F);
    t->hours   = bcd_to_dec(raw[2] & 0x3F);
    t->days    = bcd_to_dec(raw[3] & 0x3F);
    t->weekday = bcd_to_dec(raw[4] & 0x07);
    t->months  = bcd_to_dec(raw[5] & 0x1F);
    t->years   = bcd_to_dec(raw[6]);

    return 0;
}

/* ==================== API publique ==================== */

static const struct device *i2c_bus;

int rtc_init(void)
{
    /* Récupération du bus I2C utilisé par les capteurs (ici i2c1) */
    i2c_bus = DEVICE_DT_GET(DT_NODELABEL(i2c1));
    if (!device_is_ready(i2c_bus)) {
        LOG_ERR("Bus I2C1 non prêt");
        return -ENODEV;
    }

    return rv8263_init(i2c_bus);
}

int rtc_set_time(const rtc_time_t *t)
{
    if (!i2c_bus) {
        LOG_ERR("RTC non initialisé");
        return -ENODEV;
    }
    return rv8263_set_time_raw(i2c_bus, t);
}

int rtc_get_time(rtc_time_t *t)
{
    if (!i2c_bus) {
        LOG_ERR("RTC non initialisé");
        return -ENODEV;
    }
    return rv8263_get_time_raw(i2c_bus, t);
}