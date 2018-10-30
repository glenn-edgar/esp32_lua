/* Task_Watchdog Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "watchdog.h"

#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("TWDT ERROR %d\n",returned);                                \
                abort();                                               \
            }                                                          \
})


void initialize_watchdog(int time_out_seconds, bool panic)
{
  printf("Initialize TWDT\n"); 
  CHECK_ERROR_CODE(esp_task_wdt_init(time_out_seconds, panic), ESP_OK);  


   

}

void wdt_task_subscribe(void)
{
 CHECK_ERROR_CODE(esp_task_wdt_add(xTaskGetCurrentTaskHandle()), ESP_OK);   
    
}

void wdt_task_unsubscribe(void)
{
    
    esp_task_wdt_delete(xTaskGetCurrentTaskHandle());
}

void wd_task_status(void)
{
    
    CHECK_ERROR_CODE(esp_task_wdt_status(xTaskGetCurrentTaskHandle()), ESP_OK);
    
}

void wdt_reset_task_time(void)
{
  CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
      
}

