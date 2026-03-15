#ifndef BLE_CONFIGURATION_H
#define BLE_CONFIGURATION_H

#include <zephyr/types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Payload pour les données environnementales (température et humidité)
 */
struct __packed ble_env_payload {
    uint32_t epoch_s;   /**< Secondes depuis le démarrage */
    int16_t  temp_cC;   /**< Température en centi‑degrés Celsius */
    uint16_t hum_cpc;   /**< Humidité en centi‑pourcents */
};

/**
 * @brief Initialise le BLE et démarre l’advertising.
 *
 * @return 0 en cas de succès, code d’erreur négatif sinon.
 */
int ble_init(void);

/**
 * @brief Envoie le payload environnemental par notification si connecté et activé.
 *
 * @param payload Pointeur vers le payload à envoyer.
 */
void ble_send_env_payload(const struct ble_env_payload *payload);

/**
 * @brief Met à jour les données d’advertising avec les valeurs courantes.
 *        N’a d’effet que lorsque la montre n’est pas connectée.
 *
 * @param payload Pointeur vers le payload contenant les valeurs.
 */
void ble_update_advertising(const struct ble_env_payload *payload);

#ifdef __cplusplus
}
#endif

#endif /* BLE_CONFIGURATION_H */