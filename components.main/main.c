#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"

#include <esp_types.h>



#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_vfs_fat.h"
#include "esp_task_wdt.h"
#include "esp_eth.h"
#include "esp_log.h"
#include <freertos/task.h>

#include "driver/uart.h"
#include "rom/uart.h"
#include "flash_setup.h"
#include "sd_card.h"
#include "message_processor.h"


#include "configuration.h"
#include "gpio_heartbeat.h"
#include "serial_heartbeat.h"

#if _ENABLE_ETHERNET_ 
#include "ethernet/olimex_ethernet.h"
#endif

#if _ENABLE_WATCHDOG_
#include "watchdog.h"

#endif

#if _ENABLE_HEART_BEAT_

#include "gpio.h"

#endif

#if _ENABLE_WIFI_
#include "wifi_station_setup.h"
#endif


#include "internal_temp.h"

#include "spiffs_setup.h"
#include "console_output_server.h"
#include "console_input_server.h"
#include "mqtt_ctrl.h"
#include "app_input_main.h"
#include "app_output_main.h"
#include "app_pwm_setup.h"
#include "app_pulse_setup.h"
#include "app_analog_main.h"
#include "esp_spiram.h"
#include "lua_main.h"

                                
void app_main(void)
{

    char *test_mem;
    printf("-------REBOOT------------\n");
    
    if( initialize_spiffs() == false )
    {
      // bad flash cannot go further
      esp_restart();    
        
    }
#if _ENABLE_SD_CARD_
    initialize_sd_card();
    //test_file_system();
   
   

#endif
    //test_mem = heap_caps_malloc(100000, MALLOC_CAP_SPIRAM);
    //printf("test_mem %p \n",test_mem);
  
    flash_initialize_flash();
    init_message_processor();
    console_output_server();
   initialize_console_input_server();
#if _ENABLE_ETHERNET_ 
    initEthernet();
#endif

#if _ENABLE_WIFI_
    wifi_init_sta();
#endif
 

    
    initialize_watchdog(20, true);
    wdt_task_subscribe();
    
   initialize_serial_heart_beat();
   initialize_mqtt_client_handler();

   initialize_app_input_main(); 
   initialize_app_output_main(); 
   initialize_app_pwm_main();
   initialize_app_pulse_main();
   initialize_app_analog_main();
   lua_initialize_main();
    while(1)
    {
      
      
#if 0
      printf("high water stack mark %d \n",uxTaskGetStackHighWaterMark( NULL ));
      printf("heap space %d \n",esp_get_free_heap_size());
#endif

      wdt_reset_task_time();
      vTaskDelay(4000 / portTICK_PERIOD_MS);
        
       
        
    }
}

