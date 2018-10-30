#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "math.h"
#include <esp_types.h>
#include "esp_event.h"
#include "freertos/task.h"
#include "watchdog.h"
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "msg_dict_stream.h"
#include "app_analog_event_queue.h"
#include "app_analog_mqtt_update.h"

#define TWENTY_SECONDS 20*100

#define MESSAGE_PACK_NUMBER 2+4*ANALOG_CHANNEL_NUMBER   

static APP_MQTT_STRUCTURE mqtt_structure;
static MSG_PACK_ELEMENT msg_pack[MESSAGE_PACK_NUMBER];

static xQueueHandle mqtt_queue;

static void mqtt_packaging_queue(void *arg);

void app_analog_initialize_mqtt_task(  xQueueHandle mqtt_que )
{
    
    mqtt_queue = mqtt_que;
    xTaskCreate(mqtt_packaging_queue, "analog_mqtt_task", 4000, NULL, 10, NULL);
    
}




static void mqtt_packaging_queue(void *arg)
{
    int buffer_size;
    char *buffer;
    float sd_average;
    uint32_t pack_index =0;

    task_wait_for_mqtt_connection();
    wdt_task_subscribe();
    
    while (1) 
    {
        wdt_reset_task_time();
        if( xQueueReceive(mqtt_queue, &mqtt_structure,TWENTY_SECONDS) == 0){continue;}
        
        pack_index = 0;
        msg_dict_pack_string(&msg_pack[pack_index++],"TOPIC","INPUT/AD1/VALUE");
       
        msg_dict_pack_array(&msg_pack[pack_index++],"MEASUREMENTS", mqtt_structure.channel_number);
        for(int i = 0; i<mqtt_structure.channel_number; i++)
        {

            sd_average = sqrt(mqtt_structure.app_analog_mqtt_channel[i].sd_average);
            msg_dict_pack_map(&msg_pack[pack_index++],NULL, 3);
            msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"CHANNEL",
                    mqtt_structure.app_analog_mqtt_channel[i].ad_channel);
              
            msg_dict_pack_float(&msg_pack[pack_index++], "DC",mqtt_structure.app_analog_mqtt_channel[i].dc_average);
            msg_dict_pack_float(&msg_pack[pack_index++],"SD",sd_average);
             
        }
        
        buffer =  msg_dict_stream( &buffer_size, pack_index, msg_pack);
        mqtt_clt_publish("INPUT/AD1/VALUE", buffer,buffer_size );
        free(buffer);
  
    }
}