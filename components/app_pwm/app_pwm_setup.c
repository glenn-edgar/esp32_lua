#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "driver/mcpwm.h"
#include "msgpack_rx_handler.h"
#include "app_output_read_config.h"
#include "msg_dict_stream.h"
#include "app_pwm_setup.h"



#define MCPWM_PIN_IGNORE    (-1)




typedef struct
{
    uint32_t unit_id;
    uint32_t pin_a;
    uint32_t pin_b; // MCPWM_PIN_IGNORE if not used
    uint32_t frequency;
    float   duty_a;
    float   duty_b;
   
}PWM_CHANNEL_T;



uint32_t      pwm_channel_number;
PWM_CHANNEL_T pwm_channel[MCPWM_TIMER_MAX];




static void app_pwm_change_duty( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  );
    

static void app_pwm_read_duty( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  );
    
static bool app_pwm_read_file_configuration( void );


static bool  app_pwm_find_set_configuration(  uint32_t data_len, char *data);
static bool  app_pwm_find_set_configuration_a(  uint32_t data_len, char *data);
static void initialize_timer_units(void);


                             
static bool app_pwm_process_buffer_a(uint32_t index,uint32_t data_len , char *data);
static bool app_pwm_process_buffer(uint32_t timer_id,uint32_t data_len , char *data);                             
 
                   
                      
                      



                             
void initialize_app_pwm_main(void)
{
    pwm_channel_number = 0;
    memset(pwm_channel,0,sizeof(pwm_channel));
    initialize_timer_units();
   
    if( app_pwm_read_file_configuration( ) == true)
    {
       mqtt_ctrl_register_subscription("OUTPUTS/PWM/CHANGE_DUTY", app_pwm_change_duty );
       mqtt_ctrl_register_subscription("OUTPUTS/PWM/REQUEST_DUTY", app_pwm_read_duty );
    }
        
}
    
static void app_pwm_change_duty( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  )
{


  app_pwm_find_set_configuration_a( data_len, data); 
                        
 
       
          
}                          


static void app_pwm_read_duty(  esp_mqtt_client_handle_t mqtt_client,
                                 uint32_t topic_len,
                                 char *topic, 
                                 uint32_t data_len, 
                                char *data  )
                                

{
   MSG_PACK_ELEMENT *msg_pack;
   uint32_t  pack_index= 0;
   uint32_t  msg_pack_number;
   int       pack_buffer_size;
   char      *pack_buffer;
   
 
   msg_pack_number = 6*pwm_channel_number+3;
   msg_pack_number +=4; //safety
   pack_index = 0;
   
   msg_pack = malloc(msg_pack_number*sizeof(MSG_PACK_ELEMENT));
   
   msg_dict_pack_string(&msg_pack[pack_index++],"TOPIC","OUTPUTS/PWM/READ_DUTY"); 
   
  
   msg_dict_pack_array(&msg_pack[pack_index++],"DATA",pwm_channel_number);
   for(  int i = 0;i<pwm_channel_number;i++)
   {
      msg_dict_pack_map(&msg_pack[pack_index++],NULL, 5);

      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"GPIO_PIN_A", pwm_channel[i].pin_a);
      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"GPIO_PIN_B", pwm_channel[i].pin_b);

      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"FREQUENCY", pwm_channel[i].frequency);

      msg_dict_pack_float(&msg_pack[pack_index++],"DUTY_A", pwm_channel[i].duty_a);
      msg_dict_pack_float(&msg_pack[pack_index++],"DUTY_B", pwm_channel[i].duty_b);
      
       
   }
   
 
   pack_buffer = msg_dict_stream( &pack_buffer_size,pack_index,msg_pack);
   
   mqtt_clt_publish("OUTPUTS/PWM/READ_DUTY", pack_buffer,pack_buffer_size );
   free(pack_buffer);
   free(msg_pack);
 
    
    
}      

 
                                

static bool app_pwm_read_file_configuration( void ) 
{
    bool          return_value;
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
       
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_PWM.MPK" ,  &buffer,&buffer_size ) != true)
    {
        
         return false;
    }
   
    
    return_value = app_pwm_find_set_configuration(   buffer_size, buffer);
    
    free(buffer);
    return return_value;    
    
}

 

static void initialize_timer_units( void )
{
  PWM_CHANNEL_T *temp;

  temp = pwm_channel;
  for(int i = 0; i<MCPWM_TIMER_MAX;i++)
  {
      temp->pin_a = MCPWM_PIN_IGNORE; 
      temp->pin_b = MCPWM_PIN_IGNORE;
      if( i < 3 )
      {
          temp->unit_id = 0;
      }
      else
      {
          temp->unit_id = 1;
      }
      
      
  }      
    
}    


   
                   

   
    
                          
/*
**
** MESSAGE PACK STUFF
**
**
*/

   
                      

static bool app_pwm_process_buffer(uint32_t timer_id,uint32_t data_len , char *data)
{
   cmp_ctx_t ctx;
   uint32_t pin_a;
   uint32_t pin_b;
   uint32_t frequency;
   float    duty_a;
   float    duty_b;
   msgpack_rx_handler_init(&ctx, data, data_len); 
   
   
   if( msgpack_rx_handler_find_float(&ctx,"duty_a", &duty_a ) == false)
   {
       return false;
       
   }  
    
   if( msgpack_rx_handler_find_float(&ctx,"duty_b", &duty_b )== false)
   {
       return false;
       
   }   
   
   if( msgpack_rx_handler_find_unsigned(&ctx,"frequency", &frequency ) == false)
   {
       return false;
       
   }  
     
   if( msgpack_rx_handler_find_unsigned(&ctx,"pin_a", &pin_a ) == false)
   {
       return false;
       
   }   
    
    if( msgpack_rx_handler_find_unsigned(&ctx,"pin_b", &pin_b ) == false)
   {
       return false;
       
   }   
   
   pwm_channel[timer_id].pin_a = pin_a;
   pwm_channel[timer_id].pin_b = pin_b;
   pwm_channel[timer_id].frequency = frequency;
   pwm_channel[timer_id].duty_a = duty_a;
   pwm_channel[timer_id].duty_b = duty_b;

   mcpwm_config_t pwm_config;
   pwm_config.frequency = frequency;    
   pwm_config.cmpr_a = duty_a;       
   pwm_config.cmpr_b = duty_b;      
   pwm_config.counter_mode = MCPWM_UP_COUNTER;
   pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

   if( timer_id >= 3)
   {
      mcpwm_gpio_init(1, timer_id*2, pin_a);
      mcpwm_gpio_init(1, (timer_id*2)+1, pin_b);
      mcpwm_init(1, timer_id-3, &pwm_config); 
   }
   else
   {
      mcpwm_gpio_init(0, timer_id*2, pin_a);
      mcpwm_gpio_init(0, (timer_id*2)+1, pin_b);
      mcpwm_init(0, timer_id, &pwm_config); 
   }
       
   
    
   return true;
    
}



static bool  app_pwm_find_set_configuration(  uint32_t data_len, char *data)
{ 

    uint32_t binary_length[ MCPWM_TIMER_MAX ];
    char   *binary_buffer[ MCPWM_TIMER_MAX];

    cmp_ctx_t ctx;
    uint32_t pwm_count;
    
    msgpack_rx_handler_init(&ctx, data, data_len);
    
    if( msgpack_rx_handler_find_array_count(&ctx,"PWM_OUTPUTS",&pwm_count) != true )
    {
        return false;
    }
   
    if(pwm_count >= MCPWM_TIMER_MAX){ return false; }
    pwm_channel_number = pwm_count;
    
   
    if(msgpack_rx_handler_get_binary_array(&ctx,"PWM_OUTPUTS",&pwm_count,binary_length,binary_buffer) == false)
    {
       
        return false;
    }

    for(int i = 0; i< pwm_count ;i++)
    {
       
       if( app_pwm_process_buffer(i, binary_length[i],binary_buffer[i]) == false)
       {
           
           return false;
       }
       
    }

   
    return true;       
    
    
}

static bool  app_pwm_find_set_configuration_a(  uint32_t data_len, char *data)
{ 

    uint32_t binary_length[ MCPWM_TIMER_MAX ];
    char   *binary_buffer[ MCPWM_TIMER_MAX];

    cmp_ctx_t ctx;
    uint32_t pwm_count;
    
    msgpack_rx_handler_init(&ctx, data, data_len);
    
    if( msgpack_rx_handler_find_array_count(&ctx,"PWM_OUTPUTS",&pwm_count) != true )
    {
        return false;
    }
   
    if(pwm_count >= MCPWM_TIMER_MAX){ return false; }
    pwm_channel_number = pwm_count;
    
   
    if(msgpack_rx_handler_get_binary_array(&ctx,"PWM_OUTPUTS",&pwm_count,binary_length,binary_buffer) == false)
    {
       
        return false;
    }
  
    for(int i = 0; i< pwm_count ;i++)
    {
      
       if( app_pwm_process_buffer_a(i, binary_length[i],binary_buffer[i]) == false)
       {
           
           return false;
       }
       
    }

  
    return true;       
    
    
}

static bool app_pwm_process_buffer_a(uint32_t index,uint32_t data_len , char *data)
{
   cmp_ctx_t ctx;
   uint32_t timer_id;
   float    duty_a;
   float    duty_b;
   msgpack_rx_handler_init(&ctx, data, data_len); 
   
   
   if( msgpack_rx_handler_find_float(&ctx,"duty_a", &duty_a ) == false)
   {
       return false;
       
   }  
    
   if( msgpack_rx_handler_find_float(&ctx,"duty_b", &duty_b )== false)
   {
       return false;
       
   }   
   if(msgpack_rx_handler_find_unsigned(&ctx,"timer_id",&timer_id) == false)
   {
       return false;
   }    
   if( timer_id >= pwm_channel_number)
   {
       return false;
   }
   pwm_channel[timer_id].duty_a = duty_a;
   pwm_channel[timer_id].duty_b = duty_b;
   
   if(timer_id < 3)
   {
       mcpwm_set_duty(0, timer_id,timer_id*2, duty_a);
       mcpwm_set_duty(0, timer_id, (timer_id*2)+1, duty_b);
   }
   else
   {
       timer_id = timer_id -3;
       mcpwm_set_duty(1, timer_id,timer_id*2, duty_a);
       mcpwm_set_duty(1, timer_id, (timer_id*2)+1, duty_b);       
       
   }
    
   return true;
    
}

