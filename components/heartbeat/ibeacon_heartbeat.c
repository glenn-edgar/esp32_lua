

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>

#include "watchdog.h"
#include "console_output_server.h"
#include "status_interface.h"
#include "ble_custom_beacon.h"
#include "ibeacon_heartbeat.h"

static TaskHandle_t xHandle = NULL;

// task has about 544 bytes free with 2000 byte stack 

static void ibeacon_heartbeat_task( void * pvParameters );
void initialize_ibeacon_heart_beat(void)
{
    init_status_interface();
    xTaskCreate( ibeacon_heartbeat_task, "IBEACON_HEART_BEAT",2000, NULL, 10, &xHandle );
    
}

static void ibeacon_heartbeat_task( void * pvParameters )
{

  uint32_t   status= 0;
  ibeacon_custom_setup();
  wdt_task_subscribe();
  status = get_status_value();
  ibeacon_custom_start( 100, (status>>16)&0xffff,status&0xffff);
  for( ;; )
  {
   wdt_reset_task_time();
   
   //status = get_status_value();
   status += 1;
   
   ibeacon_custom_change_numbers((status>>16)&0xffff,status&0xffff);
   
   
   printf("high water stack mark %d \n",uxTaskGetStackHighWaterMark( NULL ));
   
   
   vTaskDelay(1000/ portTICK_PERIOD_MS);

  }
}


    
     

