#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci.h>
#include <stdio.h>
#include <string.h>
#include "BLE_Configuration.h"

/*---------------------------------------------------------------------------*
 *  UUIDs personnalisés 128 bits
 *---------------------------------------------------------------------------*/
#define BT_UUID_ENV_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x1234, 0x1234, 0x1234567890ab)
#define BT_UUID_ENV_CHAR_VAL \
    BT_UUID_128_ENCODE(0x87654321, 0x4321, 0x4321, 0x4321, 0xba0987654321)

static struct bt_uuid_128 env_svc_uuid = BT_UUID_INIT_128(BT_UUID_ENV_SERVICE_VAL);
static struct bt_uuid_128 env_chr_uuid = BT_UUID_INIT_128(BT_UUID_ENV_CHAR_VAL);

/*---------------------------------------------------------------------------*
 *  État de la connexion et des notifications
 *---------------------------------------------------------------------------*/
static struct bt_conn *g_conn;
static bool g_notify_enabled;

/*---------------------------------------------------------------------------*
 *  Callback de configuration CCC (activation/désactivation des notifications)
 *---------------------------------------------------------------------------*/
static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);
    g_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    printk("BLE notify %s\n", g_notify_enabled ? "enabled" : "disabled");
}

/*---------------------------------------------------------------------------*
 *  Définition du service GATT
 *---------------------------------------------------------------------------*/
BT_GATT_SERVICE_DEFINE(env_svc,
    BT_GATT_PRIMARY_SERVICE(&env_svc_uuid),
    BT_GATT_CHARACTERISTIC(&env_chr_uuid.uuid,
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

/*---------------------------------------------------------------------------*
 *  Callbacks de connexion BLE
 *---------------------------------------------------------------------------*/
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("BLE connection failed (err %u)\n", err);
        return;
    }
    g_conn = bt_conn_ref(conn);
    printk("BLE connected\n");

    /* Option : forcer la PHY 1M */
    struct bt_conn_le_phy_param phy = {
        .options = BT_CONN_LE_PHY_OPT_NONE,
        .pref_tx_phy = BT_GAP_LE_PHY_1M,
        .pref_rx_phy = BT_GAP_LE_PHY_1M,
    };
    bt_conn_le_phy_update(conn, &phy);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("BLE disconnected (reason %u)\n", reason);
    if (g_conn) {
        bt_conn_unref(g_conn);
        g_conn = NULL;
    }
    g_notify_enabled = false;
}

BT_CONN_CB_DEFINE(conn_cb) = {
    .connected    = connected,
    .disconnected = disconnected,
};

/*---------------------------------------------------------------------------*
 *  Fonction interne pour mettre à jour l’advertising avec un nom personnalisé
 *---------------------------------------------------------------------------*/
static void update_advertising(const char *name)
{
    if (g_conn) {
        /* Déjà connecté, on ne change pas l’advertising */
        return;
    }

    /* Données d’advertising (flags uniquement) */
    const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR))
    };

    /* Données de scan response (nom du périphérique) */
    struct bt_data sd;
    sd.type = BT_DATA_NAME_COMPLETE;
    sd.data_len = strlen(name);
    sd.data = (const uint8_t *)name;

    bt_le_adv_stop();
    bt_le_adv_start(BT_LE_ADV_CONN_FAST_1,
                    ad, ARRAY_SIZE(ad),
                    &sd, 1);
}

/*---------------------------------------------------------------------------*
 *  Fonctions publiques
 *---------------------------------------------------------------------------*/
int ble_init(void)
{
    int err = bt_enable(NULL);
    if (err) {
        printk("BLE init failed (err %d)\n", err);
        return err;
    }

    /* Advertising initial avec le nom par défaut (Kconfig) */
    const char *default_name = CONFIG_BT_DEVICE_NAME;
    update_advertising(default_name);
    printk("BLE advertising started with name: %s\n", default_name);
    return 0;
}

void ble_send_env_payload(const struct ble_env_payload *payload)
{
    if (!g_conn || !g_notify_enabled || !payload) {
        return;
    }

    int err = bt_gatt_notify(g_conn, &env_svc.attrs[1],
                              payload, sizeof(*payload));
    if (err) {
        printk("bt_gatt_notify error %d\n", err);
    } else {
        printk("BLE notification sent\n");
    }
}

void ble_update_advertising(const struct ble_env_payload *payload)
{
    if (!payload) return;

    /* Construction d’un nom court avec température et humidité */
    char name[30];
    int16_t temp_abs = (payload->temp_cC < 0) ? -payload->temp_cC : payload->temp_cC;
    snprintf(name, sizeof(name), "T:%s%d.%02d H:%d.%02d",
             (payload->temp_cC < 0) ? "-" : "",
             temp_abs / 100, temp_abs % 100,
             payload->hum_cpc / 100, payload->hum_cpc % 100);

    update_advertising(name);
}