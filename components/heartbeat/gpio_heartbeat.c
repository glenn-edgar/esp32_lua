
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>
#include "gpio.h"
#include "watchdog.h"

static TaskHandle_t xHandle = NULL;

// task has about 544 bytes free with 2000 byte stack 

static void gpio_heartbeat_task( void * pvParameters );
void initialize_gpio_heart_beat(int gpio_pin)
{
    // setup hardarwar
    xTaskCreate( gpio_heartbeat_task, "GPIO_HEART_BEAT",2000, (void*)gpio_pin, 10, &xHandle );
    
}

static void gpio_heartbeat_task( void * pvParameters )
{
  uint32_t gpio_pin;
  uint32_t heartbeat_toggle;
  
  heartbeat_toggle = 1;
  
  gpio_pin =  (uint32_t)pvParameters;
  printf("gpio_pin %d \n",gpio_pin);
  gpio_config_output_pin( gpio_pin );
  wdt_task_subscribe();

  for( ;; )
  {
   wdt_reset_task_time();
   heartbeat_toggle = (heartbeat_toggle^0xffff)&0x0001;
   gpio_set_value(gpio_pin,heartbeat_toggle);
   
   vTaskDelay(1000/ portTICK_PERIOD_MS);

  }
}
    
     

