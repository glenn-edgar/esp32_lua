#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "watchdog.h"
#include "gpio.h"
#include "debounce_ctrl.h"
#include "mqtt_ctrl.h"
#include "chain_flow_support.h"


int pat_watch_dog(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
   wdt_reset_task_time(); 
   return CF_DISABLE;   
}


int add_watch_dog(CHAIN_FLOW_HANDLE *cf,unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    
   wdt_task_subscribe(); 
   return CF_DISABLE;     
}  


int wait_for_mqtt_connect(CHAIN_FLOW_HANDLE *cf,unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)

{
    MQTT_STATE  state;
    int return_value;
    state = mqtt_ctl_get_state();
    if( (state == RESTORE_SUBSCRIPTIONS) || ( state ==MQTT_OPERATIONAL_STATE ))
    {
    return_value = 1;
    }
    else
    {
        return_value = 0;
    }
    return return_value;
}    
