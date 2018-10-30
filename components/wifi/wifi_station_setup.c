#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_station_setup.h"
#include "msgpack_rx_handler.h"

static esp_err_t event_handler(void *ctx, system_event_t *event);

#define NAME_SIZE 32

char ssid[NAME_SIZE];
char password[NAME_SIZE];
char hostname[NAME_SIZE];
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
   
const int WIFI_CONNECTED_BIT = BIT0;
wifi_config_t wifi_config;

#define TAG "WIFI_STATION_SETUP"

static bool wifi_setup_status =false;

bool wifi_get_wifi_setup_status(void)
{
    return wifi_setup_status;
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        //ESP_LOGI(TAG, "got ip:%s",
        //         ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
          wifi_setup_status =true;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        //ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
        //         MAC2STR(event->event_info.sta_connected.mac),
          //       event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        //ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
        //         MAC2STR(event->event_info.sta_disconnected.mac),
         //        event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        wifi_setup_status =false;
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}




void wifi_init_sta(void)
{
    bool ret;
    char     *buffer;
    uint32_t  buffer_size;
    wifi_setup_status = false;
    cmp_ctx_t ctx;
    
    buffer_size = 256;
    
    ret = msgpack_rx_handler_file(&ctx,"/spiffs/WIFI.MPK",&buffer,&buffer_size );
    if(ret == false)
    {
        printf("wifi file not found \n");
        return ;
    }
   
    buffer_size = sizeof(ssid);
    ret = msgpack_rx_handler_find_string( &ctx,"ssid", ssid, &buffer_size );
    if(ret == false)
    {
        goto end;
        
    }
    
    buffer_size = sizeof(hostname);
    ret = msgpack_rx_handler_find_string( &ctx,"hostname", hostname, &buffer_size );
    if( ret == false )
    {
       goto end;
    }
    
    buffer_size = sizeof(hostname);
    ret = msgpack_rx_handler_find_string(&ctx, "password", password, &buffer_size );
    if( ret == false)
    {
       goto end;
    }
    
    wifi_event_group = xEventGroupCreate();

    strcpy((char*)wifi_config.sta.ssid, ssid );
    strcpy((char*)wifi_config.sta.password,password);

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    ESP_ERROR_CHECK( tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, hostname ) );
    printf( "wifi_init_sta finished.\n");
   
end:
    free(buffer);
}


