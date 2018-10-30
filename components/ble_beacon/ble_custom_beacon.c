
/*
adaption of espressif ibeacon demo

*/




/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



/****************************************************************************
*
* This file is for iBeacon demo. It supports both iBeacon sender and receiver
* which is distinguished by macros IBEACON_SENDER and IBEACON_RECEIVER,
*
* iBeacon is a trademark of Apple Inc. Before building devices which use iBeacon technology,
* visit https://developer.apple.com/ibeacon/ to obtain a license.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "msgpack_rx_handler.h"
#include "ble_custom_beacon.h"

#include "freertos/FreeRTOS.h"

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))
#define ESP_UUID    {0xFD, 0xA5, 0x06, 0x93, 0xA4, 0xE2, 0x4F, 0xB1, 0xAF, 0xCF, 0xC6, 0xEB, 0x07, 0x64, 0x78, 0x25}
#define ESP_MAJOR   10167
#define ESP_MINOR   61958


typedef struct {
    uint8_t flags[3];
    uint8_t length;
    uint8_t type;
    uint16_t company_id;
    uint16_t beacon_type;
}__attribute__((packed)) esp_ble_ibeacon_head_t;

typedef struct {
    uint8_t proximity_uuid[16];
    uint16_t major;
    uint16_t minor;
    int8_t measured_power;
}__attribute__((packed)) esp_ble_ibeacon_vendor_t;


typedef struct {
    esp_ble_ibeacon_head_t ibeacon_head;
    esp_ble_ibeacon_vendor_t ibeacon_vendor;
}__attribute__((packed)) esp_ble_ibeacon_t;


static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_NONCONN_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_ble_ibeacon_vendor_t vendor_config;

static const uint8_t uuid_zeros[ESP_UUID_LEN_128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* For iBeacon packet format, please refer to Apple "Proximity Beacon Specification" doc */
/* Constant part of iBeacon data */
static esp_ble_ibeacon_head_t ibeacon_common_head = {
    .flags = {0x02, 0x01, 0x06},
    .length = 0x1A,
    .type = 0xFF,
    .company_id = 0x004C,
    .beacon_type = 0x1502
};

/* Vendor part of iBeacon data*/
static esp_ble_ibeacon_vendor_t vendor_config = {
    .proximity_uuid = ESP_UUID,
    .major = ENDIAN_CHANGE_U16(ESP_MAJOR), //Major=ESP_MAJOR
    .minor = ENDIAN_CHANGE_U16(ESP_MINOR), //Minor=ESP_MINOR
    .measured_power = 0xC5
};





static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void ble_ibeacon_appRegister(void);
static void ble_ibeacon_init(void);
static void setup_major_minor(uint16_t major, uint16_t minor);
static void setup_broadcast_interval(int broad_cast_interval_msec);
static esp_err_t esp_ble_config_ibeacon_data (esp_ble_ibeacon_vendor_t *vendor_config, 
                                              esp_ble_ibeacon_t *ibeacon_adv_data);

void ibeacon_custom_setup(void)
{
    bool ret;
    char     *buffer;
    uint32_t  buffer_size;
    
    cmp_ctx_t ctx;
    char      ibuffer_name[17];
    
    buffer_size = 256;
    
    ret = msgpack_rx_handler_file(&ctx,"/spiffs/IBEACON.MPK",&buffer,&buffer_size );
    if(ret == false)
    {
        return ;
    }
   
    buffer_size = 17;
    ret = msgpack_rx_handler_find_string( &ctx,"beacon_name", ibuffer_name, &buffer_size );
    if(ret == false)
    {
        goto end;
        
    }
    memcpy(vendor_config.proximity_uuid,ibuffer_name,16);
 
end:
    free(buffer);
}



void ibeacon_custom_stop(void)
{
    esp_bt_controller_disable();
    
}

void ibeacon_custom_start(int broadcast_interval_msec, uint16_t major, uint16_t minor)
{
    setup_major_minor(major,minor);
    setup_broadcast_interval(broadcast_interval_msec);
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    ble_ibeacon_init();

    /* set scan parameters */



    esp_ble_ibeacon_t ibeacon_adv_data;
    esp_err_t status = esp_ble_config_ibeacon_data (&vendor_config, &ibeacon_adv_data);
    if (status == ESP_OK){
        esp_ble_gap_config_adv_data_raw((uint8_t*)&ibeacon_adv_data, sizeof(ibeacon_adv_data));
    }
    else {
        printf("Config iBeacon data failed: %s\n", esp_err_to_name(status));
    }

}

void ibeacon_custom_change_numbers(uint16_t major, uint16_t minor)
{
    setup_major_minor(major,minor);
    esp_ble_ibeacon_t ibeacon_adv_data;
    esp_err_t status = esp_ble_config_ibeacon_data (&vendor_config, &ibeacon_adv_data);
    if (status == ESP_OK){
        esp_ble_gap_config_adv_data_raw((uint8_t*)&ibeacon_adv_data, sizeof(ibeacon_adv_data));
    }
    else {
        printf("Config iBeacon data failed: %s\n", esp_err_to_name(status));
    }    
    
}




static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;
    
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        
        esp_ble_gap_start_advertising(&ble_adv_params);

        break;
  



    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        printf("made it here 555555555555\n");
        if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            printf( "Adv stop failed: %s", esp_err_to_name(err));
        }
        else {
            printf( "Stop adv successfully");
        }
        break;

    default:
        break;
    }
}


static void ble_ibeacon_appRegister(void)
{
    esp_err_t status;

    

    //register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        printf( "gap register error: %s", esp_err_to_name(status));
        return;
    }

}

static void ble_ibeacon_init(void)
{
    esp_bluedroid_init();
    esp_bluedroid_enable();
    ble_ibeacon_appRegister();
}






#if 0 // remove warning
static bool esp_ble_is_ibeacon_packet (uint8_t *adv_data, uint8_t adv_data_len){
    bool result = false;

    if ((adv_data != NULL) && (adv_data_len == 0x1E)){
        if (!memcmp(adv_data, (uint8_t*)&ibeacon_common_head, sizeof(ibeacon_common_head))){
            result = true;
        }
    }

    return result;
}
#endif

static esp_err_t esp_ble_config_ibeacon_data (esp_ble_ibeacon_vendor_t *vendor_config, 
                                              esp_ble_ibeacon_t *ibeacon_adv_data)
{
    if ((vendor_config == NULL) || (ibeacon_adv_data == NULL) || (!memcmp(vendor_config->proximity_uuid, uuid_zeros, sizeof(uuid_zeros)))){
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&ibeacon_adv_data->ibeacon_head, &ibeacon_common_head, sizeof(esp_ble_ibeacon_head_t));
    memcpy(&ibeacon_adv_data->ibeacon_vendor, vendor_config, sizeof(esp_ble_ibeacon_vendor_t));

    return ESP_OK;
}

static void setup_major_minor(uint16_t major, uint16_t minor)
{
    
   vendor_config.major =  ENDIAN_CHANGE_U16(major);
   vendor_config.minor =  ENDIAN_CHANGE_U16(minor);
    
}
static void setup_broadcast_interval(int broad_cast_interval_msec)
{
    uint16_t units;
    
    units = (float)broad_cast_interval_msec/.625;
    ble_adv_params.adv_int_min = units;
    ble_adv_params.adv_int_max = units*1.25;

    
}

