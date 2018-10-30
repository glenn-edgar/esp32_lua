
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
#include "app_input_read_config.h"
#include "chain_flow_assembler/cf_external_functions.h"
#include "app_input_functions.h"



static uint32_t input_number;
static uint32_t *gpio_input_pins;
static DEBOUNCE_CTRL *debounce_control;

static void app_input_configure_cb(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
static void app_input_read_cb(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
       
static void dump_gpio_values(uint32_t number, uint32_t *pins);


bool app_input_input_data_read(void)
{
  bool return_value;
  input_number = 0;
  return_value = app_input_file_read_configuration( &input_number, &gpio_input_pins,&debounce_control );
#if 0  
  printf("************** %d %d \n",return_value, input_number);
  for(int i = 0; i< input_number;i++)
  {
      printf("gpio_input_pins %d %d \n",i,gpio_input_pins[i]);
      printf("debounce.current.level %d \n",debounce_control[i].current_level);
      printf("debounce.reference_level %d \n",debounce_control[i].reference_level);
      printf("debounce.current_count %d \n",debounce_control[i].current_count);
      printf("debounce.transition_count %d \n",debounce_control[i].transition_count);

  }
  vTaskDelay(100/ portTICK_PERIOD_MS); // allow print to display
#endif  
  return return_value;  
    
    
}

static void send_mqtt_message(int pin,int value);
bool debounce_control_update( DEBOUNCE_CTRL *debounce_ctrl, unsigned new_value);



int app_input_sample_inputs(CHAIN_FLOW_HANDLE *cf,
                            unsigned link_id, 
                            unsigned param_1,
                            unsigned param_2, 
                            unsigned param_3, 
                            unsigned event, 
                            unsigned data)
{
    unsigned input;
    bool     transistion;
    
    for( int i=0;i<input_number;i++)
    {
        input = gpio_read_value(gpio_input_pins[i]);
        transistion = debounce_control_update( &debounce_control[i], input);
        if(transistion == true)
        {
            send_mqtt_message(gpio_input_pins[i],input);
        }
        
    }
    return CF_DISABLE;    
}

 
      
int app_input_register_read(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    
    mqtt_ctrl_register_subscription("INPUT/GPIO/READ", app_input_read_cb );
    return CF_DISABLE;        
    
    
}


int app_input_register_configure_read(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
 
    mqtt_ctrl_register_subscription("INPUT/GPIO/CONFIGURE", app_input_configure_cb );
    return CF_DISABLE;        
  
      
      
}



static void app_input_read_cb(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
{
    uint32_t input_number;
    uint32_t *pins;  
    
    if( app_input_read_input( data_len, data, &input_number, &pins ) == true)
    {
      
      dump_gpio_values(input_number, pins);
      free(pins)  ;         
        
        
    }
    else
    {
       ; 
    }

 
    
    
}   
                            
static void app_input_configure_cb(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
{
    
 uint32_t input_number;
 uint32_t *pins;
 DEBOUNCE_CTRL *debounce_ctrl; 
 if( app_input_read_setup( data_len, data,&input_number, &pins,&debounce_ctrl ) == true )
 {
    
    dump_gpio_values(input_number, pins);
    free(pins)  ; 
    free(debounce_ctrl);    
     
     
 }     
    
    
}
                                
static void dump_gpio_values(uint32_t number, uint32_t *pins)
{
   MSG_PACK_ELEMENT *msg_pack;
   uint32_t  pack_index= 0;
   uint32_t  msg_pack_number;
   int       pack_buffer_size;
   char      *pack_buffer;
   
   msg_pack_number = 2*number+4;
   msg_pack_number +=4; //safety
   
   msg_pack = malloc(msg_pack_number*sizeof(MSG_PACK_ELEMENT));
   
   msg_dict_pack_string(&msg_pack[pack_index++],"TOPIC","INPUT/GPIO/VALUE"); 
   msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"NUMBER", number);
   
   msg_dict_pack_array(&msg_pack[pack_index++],"PINS",number);
   for(  int i = 0;i<number;i++)
   {
      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],NULL,(long)pins[i]);
       
       
   }
   
   msg_dict_pack_array(&msg_pack[pack_index++],"VALUE",number);
   for( int i = 0;i<number;i++)
   {
     
       msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],NULL,(long)gpio_read_value( pins[i]));     
       
   }
   
   pack_buffer = msg_dict_stream( &pack_buffer_size,pack_index,msg_pack);
   printf("pack_buffer_size %d \n",pack_buffer_size);
   mqtt_clt_publish("INPUT/GPIO/VALUE", pack_buffer,pack_buffer_size );
   free(pack_buffer);
   free(msg_pack);
 
    
    
}      





static void send_mqtt_message(int pin,int value)
{
   MSG_PACK_ELEMENT msg_pack[5];
   char *pack_buffer;
   int pack_buffer_size;
  
   msg_dict_pack_string(&msg_pack[0],"TOPIC","INPUT/GPIO/CHANGE");      
   msg_dict_pack_unsigned_integer(&msg_pack[1],"PIN",pin );
   msg_dict_pack_unsigned_integer(&msg_pack[2],"INPUT",value);
   pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
   mqtt_clt_publish("INPUT/GPIO/CHANGE", pack_buffer,pack_buffer_size );
   free(pack_buffer);
}
   
    
    
    
    

