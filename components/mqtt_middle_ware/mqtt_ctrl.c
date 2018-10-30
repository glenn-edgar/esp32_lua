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
#include "freertos/task.h"
#include "mqtt_client.h"
#include "mqtt_ctrl_load_setup_data.h"
#include "wifi_station_setup.h"
#include "mqtt_ctrl_subscription.h"
#include "mqtt_ctrl.h"

static TaskHandle_t xHandle = NULL;
static SemaphoreHandle_t xSemaphore = NULL;
static esp_mqtt_client_handle_t mqtt_client;
static bool reboot_flag = true;


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
static esp_mqtt_client_config_t mqtt_cfg;
 


static void mqtt_client_task( void * pvParameters );





void initialize_mqtt_client_handler( void)
{
  vSemaphoreCreateBinary( xSemaphore );  
  xTaskCreate( mqtt_client_task, "MQTT_CLIENT_TASK",16000, NULL, 20, &xHandle );
    configASSERT( xHandle );

    
}

//bool mqtt_ctl_register_subscription( TBD);
//bool mqtt_ctl_publish_topic( char *topic, char *data );



/*
********************************  static functions *****************************************
*/





static MQTT_STATE mqtt_ctl_state;
#define BASE_TOPIC_SIZE 256
static char   base_topic[BASE_TOPIC_SIZE];
static uint32_t base_topic_size;





static void mqtt_ctl_wait_for_wifi_connect(void); 
static void mqtt_ctl_setup_connection(void);

void task_wait_for_mqtt_connection(void)
{
    
    
    MQTT_STATE  state;
    while(1)
    {    
       state = mqtt_ctl_get_state();
       if( (state == RESTORE_SUBSCRIPTIONS) || ( state ==MQTT_OPERATIONAL_STATE ))
       {
             return;
       }    
       else
       {
           vTaskDelay(100/ portTICK_PERIOD_MS); 
       }           
    }

}
    
    
    




void mqtt_ctl_change_state( MQTT_STATE new_state)
{
   xSemaphoreTake( xSemaphore, ( TickType_t ) 10 );
   mqtt_ctl_state = new_state;
   xSemaphoreGive( xSemaphore );

   
}

MQTT_STATE mqtt_ctl_get_state(void)
{
   MQTT_STATE return_value;
   
   xSemaphoreTake( xSemaphore, ( TickType_t ) 10 );
   return_value = mqtt_ctl_state;
   xSemaphoreGive( xSemaphore );
   return return_value;

} 


bool mqtt_clt_publish(char *topic, char *data, int data_length )
{
    MQTT_STATE state;
    int  size;
    char *ref_topic;
   
    state = mqtt_ctl_get_state();
    if( (state == RESTORE_SUBSCRIPTIONS) || ( state ==MQTT_OPERATIONAL_STATE ))
    {
         size = strlen(base_topic)+strlen(topic)+4;
         ref_topic = malloc(size);
         strcpy(ref_topic,base_topic);
         
         strcat(ref_topic,topic);
         //printf("reference topic %s \n",ref_topic);
         //int esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, //int qos, int retain);
         esp_mqtt_client_publish(mqtt_client, ref_topic, data,data_length,0,0); //QOS 0 do not retain
         free(ref_topic);
         return true;   
        
    }
    return false;
    
    
}

bool mqtt_clt_subscribe(char *ref_topic)
{
    MQTT_STATE state;
    
    state = mqtt_ctl_get_state();
    if( (state == RESTORE_SUBSCRIPTIONS) || ( state ==MQTT_OPERATIONAL_STATE ))
    {
         esp_mqtt_client_subscribe(mqtt_client, ref_topic, 0);
         return true;   
        
    }
    return false;
    
    
}

// Task to be created.
static void mqtt_client_task( void * pvParameters )
{
   int temp_len;
   mqtt_ctl_change_state( LOAD_CONFIGURATION);
   while(1)
   {       

      MQTT_STATE state;
      state = mqtt_ctl_get_state();
   
      switch(state)
      {
          case LOAD_CONFIGURATION:
              memset(&mqtt_cfg,0,sizeof(mqtt_cfg));
              base_topic_size =sizeof(base_topic);
              mqtt_ctl_load_configuration(&mqtt_cfg, base_topic, &base_topic_size);
              temp_len = strlen(base_topic)-1;
              if(base_topic[temp_len] != '/')
              {
                  strcat(base_topic,"/");
              }
              mqtt_initialize_subscriptions(base_topic);
              break;
           
          case WAIT_FOR_WIFI_CONNECT:
             mqtt_ctl_wait_for_wifi_connect();
             break;
           
          case SETUP_MQTT_CONNECTION:

              mqtt_ctl_setup_connection();
             break;
           
          case WAIT_FOR_MQTT_CONNECT: // event call back will change state
              break;
           
          case RESTORE_SUBSCRIPTIONS:
              mqtt_ctl_restore_subscriptions( mqtt_client);
              mqtt_ctl_state = MQTT_OPERATIONAL_STATE;
              break;
           
          case MQTT_OPERATIONAL_STATE: // event_callback will change this state
              break;
          case MQTT_ERROR_STATE:
           vTaskDelay(10000/ portTICK_PERIOD_MS); // error state prevent excessive cpu cycles
           break;
       
       }
       vTaskDelay(10/ portTICK_PERIOD_MS);
   }
}   
       
       
       
   


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    //esp_mqtt_client_handle_t client = event->client;
    //int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            printf( "MQTT_EVENT_CONNECTED \n");
            mqtt_ctl_change_state(RESTORE_SUBSCRIPTIONS);
            if(reboot_flag == true)
            {
                  reboot_flag = false;
                  mqtt_clt_publish("REBOOT", "\xa6REBOOT", 7 ); // add data later
                  
            }
            

            break;
        case MQTT_EVENT_DISCONNECTED:
            printf( "MQTT_EVENT_DISCONNECTED \n");
            mqtt_ctl_change_state(WAIT_FOR_WIFI_CONNECT);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_PUBLISHED:
            break;
        case MQTT_EVENT_DATA:
            printf( "MQTT_EVENT_DATA \n");
            // need topic handler
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            mqtt_ctl_handle_data( mqtt_client,event->topic_len, event->topic,event->data_len, event->data  );
            break;
        case MQTT_EVENT_ERROR:
            printf( "MQTT_EVENT_ERROR \n");
            break;
    }
    return ESP_OK;
}


static void mqtt_ctl_wait_for_wifi_connect(void)
{
    if( wifi_get_wifi_setup_status() == true)
    {
        mqtt_ctl_change_state( SETUP_MQTT_CONNECTION);
        
    }
    
}
static void mqtt_ctl_setup_connection(void)
{
     printf("uri %s \n",  mqtt_cfg.uri );
     printf("host %s \n", mqtt_cfg.host);
     printf("port  %d \n", mqtt_cfg.port);
     printf("user name %s \n", mqtt_cfg.username);
     printf("password %s \n", mqtt_cfg.password );
     mqtt_ctl_change_state(WAIT_FOR_MQTT_CONNECT);
    mqtt_cfg.event_handle = mqtt_event_handler;
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
    
}






