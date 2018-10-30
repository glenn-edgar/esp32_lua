
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>

#include "watchdog.h"
#include "msg_dict_stream.h"
#include "status_interface.h"

#include "mqtt_ctrl.h"
#include "mqtt_heartbeat.h"

static TaskHandle_t xHandle = NULL;

// task has about 544 bytes free with 2000 byte stack 

static void mqtt_heartbeat_task( void * pvParameters );
void initialize_mqtt_heart_beat(void)
{
    init_status_interface();
    xTaskCreate( mqtt_heartbeat_task, "MQTT_HEART_BEAT",2000, NULL, 10, &xHandle );
    
}

static void mqtt_heartbeat_task( void * pvParameters )
{
  MSG_PACK_ELEMENT msg_pack[5];
  char *pack_buffer;
  int pack_buffer_size;

  uint32_t   status= 0;
  
  wdt_task_subscribe();

  for( ;; )
  {
   wdt_reset_task_time();
   status = get_status_value();
   
   msg_dict_pack_string(&msg_pack[0],"TOPIC","HEART_BEAT");      
   msg_dict_pack_unsigned_integer(&msg_pack[1],"STATUS",status );
   msg_dict_pack_unsigned_integer(&msg_pack[2],"FREE_HEAP",esp_get_free_heap_size());
   pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
   mqtt_clt_publish("HEART_BEAT", pack_buffer,pack_buffer_size );
   free(pack_buffer);
   
   
   vTaskDelay(10000/ portTICK_PERIOD_MS);

  }
}
    
     

