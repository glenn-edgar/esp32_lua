#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"


#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"

#include "mqtt_ctrl.h"
#include "msgpack_rx_handler.h"
#include "mqtt_client.h"
#include "mqtt_ctrl_load_setup_data.h"


#define BUFFER_SIZE    4000

#define USERNAME_SIZE   32
#define PASSWORD_SIZE    32
#define CERT_PEM_SIZE  2000
#define HOST_SIZE       128
#define TOPIC_SIZE     128







static char       host[HOST_SIZE];
static char       username[USERNAME_SIZE];
static char        password[PASSWORD_SIZE];
static uint32_t   port;


void mqtt_ctl_load_configuration(esp_mqtt_client_config_t *mqtt_cfg,char *base_topic, uint32_t *topic_size )
{

    bool ret;
    char     *buffer;
    uint32_t  buffer_size;
   
    cmp_ctx_t ctx;
    
    buffer_size = BUFFER_SIZE; 
    
    ret = msgpack_rx_handler_file(&ctx,"/spiffs/MQTT.MPK",&buffer,&buffer_size );
    if(ret == false)
    {
        printf("***********************************mqtt file not found \n");
        mqtt_ctl_change_state(MQTT_ERROR_STATE);
        return ;
    }
   
    
    ret = msgpack_rx_handler_find_unsigned(&ctx,"PORT", &port );
    if(ret == false)
    {
        printf("problem at PORT \n");
        goto end;
        
    }
    

    
    buffer_size = sizeof(username);
    ret = msgpack_rx_handler_find_string( &ctx,"USER_NAME", username, &buffer_size );
    
    if( ret == false )
    {
         printf("problem at user \n");
       goto end;
    }
    
    buffer_size = sizeof(password);
    ret = msgpack_rx_handler_find_string(&ctx, "PASSWORD", password, &buffer_size );
    if( ret == false)
    {
         printf("problem at password \n");
       goto end;
    }

    ret = msgpack_rx_handler_find_string(&ctx, "BASE_TOPIC", base_topic, topic_size);
    if( ret == false)
    {
         printf("problem at topic \n");
       goto end;
    }
    buffer_size = sizeof(host);
    ret = msgpack_rx_handler_find_string( &ctx,"HOST", host, &buffer_size );
    if(ret == false)
    {   printf("problem at host \n");
        goto end;
        
    }
    
    printf("***********************************mqtt data loaded \n");
    
    mqtt_cfg->uri = "mqtts://test.com";
    mqtt_cfg->host = host;
    mqtt_cfg->port = port;
    mqtt_cfg->username = username;
    mqtt_cfg->password = password;
 
    mqtt_ctl_change_state( WAIT_FOR_WIFI_CONNECT);
    free(buffer);
    return;
    
end:
    printf("*********************************mqtt data not loaded \n");
    mqtt_ctl_change_state( MQTT_ERROR_STATE);
    free(buffer);   
}

 