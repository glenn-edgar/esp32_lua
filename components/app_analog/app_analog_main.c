/* Timer group-hardware timer example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "app_analog_dac.h"
#include "app_analog_task.h"
#include "app_analog_mqtt_update.h"
#include "app_analog_analog_configuration.h"
#include "app_analog_event_queue.h"
#include "app_analog_main.h"



/*
 * In this example, we will test hardware timer0 and timer1 of timer group0.
 */
void initialize_app_analog_main(void)
{
    
    xQueueHandle mqtt_queue;

    app_analog_setup_dacs(); // these are only subscription
 
    if(app_analog_find_analog_configuration_data() == true )
    {

       
       mqtt_queue = xQueueCreate(10,sizeof(APP_MQTT_STRUCTURE));
       
       app_analog_initialize_mqtt_task(  mqtt_queue );
       
       app_analog_initialize_sampling_task(  mqtt_queue);
       
      
    }

}

