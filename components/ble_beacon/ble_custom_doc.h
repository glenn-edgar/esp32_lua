/*

Documentations of ibeacon parameters
Do not include in any code

*/


advertising_interval_min: [0x0020, 0x4000], default: 0x0800, unit: 0.625 msec
advertising_interval_max: [0x0020, 0x4000], default: 0x0800, unit: 0.625 msec

/// GAP BLE callback event type
typedef enum {
    ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT        = 0,       /*!< When advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT,             /*!< When scan response data set complete, the event comes */
    ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT,                /*!< When scan parameters set complete, the event comes */
    ESP_GAP_BLE_SCAN_RESULT_EVT,                            /*!< When one scan result ready, the event comes each time */
    ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT,              /*!< When raw advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,         /*!< When raw advertising data set complete, the event comes */
    ESP_GAP_BLE_ADV_START_COMPLETE_EVT,                     /*!< When start advertising complete, the event comes */
    ESP_GAP_BLE_SCAN_START_COMPLETE_EVT,                    /*!< When start scan complete, the event comes */
    ESP_GAP_BLE_AUTH_CMPL_EVT,                              /* Authentication complete indication. */
    ESP_GAP_BLE_KEY_EVT,                                    /* BLE  key event for peer device keys */
    ESP_GAP_BLE_SEC_REQ_EVT,                                /* BLE  security request */
    ESP_GAP_BLE_PASSKEY_NOTIF_EVT,                          /* passkey notification event */
    ESP_GAP_BLE_PASSKEY_REQ_EVT,                            /* passkey request event */
    ESP_GAP_BLE_OOB_REQ_EVT,                                /* OOB request event */
    ESP_GAP_BLE_LOCAL_IR_EVT,                               /* BLE local IR event */
    ESP_GAP_BLE_LOCAL_ER_EVT,                               /* BLE local ER event */
    ESP_GAP_BLE_NC_REQ_EVT,                                 /* Numeric Comparison request event */
    ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT,                      /*!< When stop adv complete, the event comes */
    ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT,                     /*!< When stop scan complete, the event comes */
    ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT,                   /*!< When set the static rand address complete, the event comes */
    ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT,                     /*!< When update connection parameters complete, the event comes */
    ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT,                /*!< When set pkt lenght complete, the event comes */
    ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT,             /*!< When  Enable/disable privacy on the local device complete, the event comes */
    ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT,               /*!< When remove the bond device complete, the event comes */
    ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT,                /*!< When clear the bond device clear complete, the event comes */
    ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT,                  /*!< When get the bond device list complete, the event comes */
    ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT,                     /*!< When read the rssi complete, the event comes */
    ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT,              /*!< When add or remove whitelist complete, the event comes */
    ESP_GAP_BLE_EVT_MAX,
} esp_gap_ble_cb_event_t;
#endif

