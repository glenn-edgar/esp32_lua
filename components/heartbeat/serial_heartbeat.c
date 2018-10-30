
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>

#include "watchdog.h"
#include "console_output_server.h"
#include "status_interface.h"
#include "serial_heartbeat.h"

static TaskHandle_t xHandle = NULL;

// task has about 544 bytes free with 2000 byte stack 

static void serial_heartbeat_task( void * pvParameters );
void initialize_serial_heart_beat(void)
{
    init_status_interface();
    xTaskCreate( serial_heartbeat_task, "SERIAL_HEART_BEAT",2000, NULL, 10, &xHandle );
    
}

static void serial_heartbeat_task( void * pvParameters )
{
  MSG_PACK_ELEMENT msg_pack[5];
  uint32_t   status= 0;
  
  wdt_task_subscribe();

  for( ;; )
  {
   wdt_reset_task_time();
   status = get_status_value();
   //printf("high water stack mark %d \n",uxTaskGetStackHighWaterMark( NULL ));
   msg_dict_pack_string(&msg_pack[0],"TOPIC","HEART_BEAT");      
   msg_dict_pack_unsigned_integer(&msg_pack[1],"STATUS",status );
   msg_dict_pack_unsigned_integer(&msg_pack[2],"FREE_HEAP",esp_get_free_heap_size());
   console_output_structured_data(3, msg_pack);
   
   vTaskDelay(10000/ portTICK_PERIOD_MS);

  }
}
    
     

