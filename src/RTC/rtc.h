#ifndef RTC_H
#define RTC_H

#include <zephyr/types.h>

/**
 * Structure représentant l'heure et la date lues/écrites sur le RV‑8263‑C8.
 * Les valeurs sont en décimal (non BCD).
 */
typedef struct {
    uint8_t seconds;   /* 0–59 */
    uint8_t minutes;   /* 0–59 */
    uint8_t hours;     /* 0–23 */
    uint8_t days;      /* 1–31 */
    uint8_t weekday;   /* 0=dimanche, 1=lundi, …, 6=samedi */
    uint8_t months;    /* 1–12 */
    uint8_t years;     /* 0–99 (correspond à 2000–2099) */
} rtc_time_t;

/**
 * @brief Initialise le RTC (RV‑8263‑C8) sur le bus I2C utilisé par les capteurs.
 * @return 0 si réussi, code d'erreur négatif sinon.
 */
int rtc_init(void);

/**
 * @brief Programme l'heure et la date dans le RTC.
 * @param t Pointeur vers la structure contenant les valeurs à écrire.
 * @return 0 si réussi, code d'erreur négatif sinon.
 */
int rtc_set_time(const rtc_time_t *t);

/**
 * @brief Lit l'heure et la date courantes du RTC.
 * @param t Pointeur vers la structure qui recevra les valeurs.
 * @return 0 si réussi, code d'erreur négatif sinon.
 */
int rtc_get_time(rtc_time_t *t);

#endif /* RTC_H */