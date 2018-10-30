
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "watchdog.h"
#include "gpio.h"
#include "debounce_ctrl.h"
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "msg_dict_stream.h"
#include "app_output_read_config.h"
#include "app_output_local_events.h"
#include "cf_events.h"
#include "chain_flow_assembler/cf_external_functions.h"
#include "app_input_functions.h"




#define CF_PULSE_EVENT CF_APP_EVENT_START+1


static CHAIN_FLOW_HANDLE *cf_ref;
static bool pulse_start = false;
static uint32_t pulse_pin;
static uint32_t pulse_starting_value;
static uint32_t pulse_ending_value;
static uint32_t pulse_time_ticks;


static void app_ouput_find_pulse_data(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
static void app_ouput_set_pin_data(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
#if 0                               
static void app_ouput_configure_pin_data(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                      
         
#endif      


int analog_output_store_cf_handle_ref(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
   cf_ref = cf;
   return CF_DISABLE;   
      
}

int analog_output_subscribe_configuration(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{

  // mqtt_ctrl_register_subscription("OUTPUTS/GPIO/CONFIGURATION", app_ouput_configure_pin_data );
   return CF_DISABLE;      
}


int analog_ouptut_subscribe_output(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
   //printf("subscribe   555555555555 \n");
   mqtt_ctrl_register_subscription("OUTPUTS/GPIO/SET", app_ouput_set_pin_data );
   return CF_DISABLE;      
}




int analog_output_subscribe_pulse_output(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    mqtt_ctrl_register_subscription("OUTPUTS/GPIO/SET_PULSE", app_ouput_find_pulse_data );

   return CF_DISABLE;      
}

int analog_output_set_initial_value(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
   pulse_start = true;
   //printf("ref value %d %d !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",pulse_pin, gpio_read_value(pulse_pin));
   gpio_set_value(pulse_pin, pulse_starting_value);
   //printf("initial value set %d #########################################\n",pulse_starting_value);
   return CF_DISABLE;      
}


int analog_output_step_timer(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
  unsigned value; 
  int      return_value;
  
  if( event == CF_INIT_EVENT )
  {
       //printf("made it to init \n");
       cf_store_cell_value( cf,link_id , 0 );
       return_value = CF_CONTINUE;

  }
  
   if( event == CF_TIME_TICK_EVENT )
   {
       value = cf_get_cell_value( cf,link_id );
       value += 1;
       cf_store_cell_value(cf,link_id, value);
       
       if( value >= pulse_time_ticks )
       {
          return_value = true;
       }
       else
       {
	   return_value = false;
       }
   }
   else
   {
      return_value = false;
   }
   return return_value;   

}



int analog_output_set_final_value(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
   
    pulse_start = false;
   gpio_set_value(pulse_pin, pulse_ending_value);
   //printf("final value set %d ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",pulse_ending_value);
   pulse_start = false;
   return CF_DISABLE;      
}








/*
******************************* static functions ******************************************
*/  
  
  


    
    
static void app_ouput_find_pulse_data(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
{
    
  
  if( pulse_start == true){return ;}
  
  if( app_output_find_pulse_data( data_len, data,
                                 &pulse_pin,
                                 &pulse_starting_value,
                                 &pulse_ending_value,
                                 &pulse_time_ticks) == true)
  {
     
     cf_send_event( cf_ref, CF_PULSE_EVENT,0 );
  
  }                              

    
}                           
                        

static void app_ouput_set_pin_data(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
{
 
  //printf("sub received $$$$$$$$$$$$$$$$$ data len %d \n",data_len);
  app_output_find_set_data( data_len, data);

    
}                           
         
#if 0             
static void app_ouput_configure_pin_data(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
{   
  app_output_find_set_configuration( data_len, data);
}
#endif
