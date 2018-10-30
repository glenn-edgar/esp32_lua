
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "debounce_ctrl.h"
#include "gpio.h"
#include "msgpack_rx_handler.h"
#include "app_output_read_config.h"

static uint32_t      pin_count;
static uint32_t    int_value_count;

static uint32_t *pin_array;
static uint32_t *int_value_array;

static bool action_pins_in_ref( int test_pin )
{
    
    for(int i = 0; i<pin_count;i++)
    {
        if( test_pin == pin_array[i])
        {
            
            return true;
        }
        
    }
    return false;
    
    
    
}

static bool verifiy_output_pins(int count, uint32_t *action_pins)
{
    if( count > pin_count ) { return false;}
    for(int i = 0; i <count;i++)
    {
        if( action_pins_in_ref(action_pins[i]) != true)
        {
            return false;
        }
        
        
        
    }
    return true;
    
    
    
}


bool app_output_read_file_configuration( void )
{
    bool          return_value;
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_OUT.MPK",  &buffer,&buffer_size ) != true)
    {
        
         return false;
    }
   
    
    return_value = app_output_find_set_configuration( buffer_size, buffer);
    
    free(buffer);
    return return_value;    
    
}



bool  app_output_find_set_configuration( uint32_t data_len, char *data)
{ 


 

   
    cmp_ctx_t ctx;
    uint32_t *pin_ptr;
    uint32_t *int_value_ptr;

    msgpack_rx_handler_init(&ctx, data, data_len);

    if( msgpack_rx_handler_find_array_count(&ctx,"pins",&pin_count) != true )
    {
        goto exit;
    }
    if( msgpack_rx_handler_find_array_count(&ctx,"init_values",&int_value_count) != true )
    {
         goto exit;
    }

    if( int_value_count != pin_count )
    {
         goto exit;
    }
   
    if( pin_count >0 )
    {
      
      pin_array = malloc(pin_count*sizeof(uint32_t));
      int_value_array = malloc(pin_count *sizeof(uint32_t));

      
      pin_ptr = pin_array;
      int_value_ptr = int_value_array;
      if( msgpack_rx_handler_find_array_unsigned(&ctx,"pins",&pin_count,pin_array) != true )
      {
           free(pin_array);
           free(int_value_array);

           goto exit;
      }
      if( msgpack_rx_handler_find_array_unsigned(&ctx,"init_values",&int_value_count,int_value_array) != true )
      {
          free(pin_array);
          free(int_value_array);
          
          goto exit;
      }
 
      for(int i=0;i<pin_count;i++)    
      {

           gpio_config_output_pin( *pin_ptr);
           gpio_set_value(*pin_ptr, *int_value_ptr);
           pin_ptr++;
           int_value_ptr++;
           
      }
      free(int_value_array);
      
      return true;
     
      
    }
exit:
   
    return false;       
    
    
}

bool  app_output_find_set_data( uint32_t data_len, char *data)
{   
    uint32_t      pin_count;
    uint32_t    int_value_count;

    uint32_t *pin_array;
    uint32_t *int_value_array;
   
    cmp_ctx_t ctx;
    uint32_t *pin_ptr;
    uint32_t *int_value_ptr;
     //printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
     msgpack_rx_handler_init(&ctx, data, data_len);
     if( msgpack_rx_handler_find_array_count(&ctx,"pins",&pin_count) != true )
    {
        goto exit;
    }
    if( msgpack_rx_handler_find_array_count(&ctx,"init_values",&int_value_count) != true )
    {
         goto exit;
    }
    if( int_value_count != pin_count )
    {
         goto exit;
    }
    
    if(pin_count >0 )
    {
      
      pin_array = malloc(pin_count*sizeof(uint32_t));
      int_value_array = malloc(pin_count *sizeof(uint32_t));

      
      pin_ptr = pin_array;
      int_value_ptr = int_value_array;
      if( msgpack_rx_handler_find_array_unsigned(&ctx,"pins",&pin_count,pin_array) != true )
      {
           free(pin_array);
           free(int_value_array);

           goto exit;
      }
      if( msgpack_rx_handler_find_array_unsigned(&ctx,"init_values",&int_value_count,int_value_array) != true )
      {
          free(pin_array);
          free(int_value_array);
          
          goto exit;
      }
      //printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
      if( verifiy_output_pins(pin_count,pin_array) == false){return false;}
      //printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
      for(int i=0;i<pin_count;i++)    
      {

          //printf("############################ pin %d \n",*pin_ptr);
          //printf("############################ pin %d \n",*int_value_ptr);
           gpio_set_value(*pin_ptr, *int_value_ptr);
           pin_ptr++;
           int_value_ptr++;
           
      }
      free(int_value_array);
      //printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
      return true;
     
      
    }
exit:
   
    return false;       
    
    
}
                                 





bool app_output_find_pulse_data( uint32_t data_len, char *data,
                                 uint32_t *pulse_pin,
                                 uint32_t *pulse_starting_value,
                                 uint32_t *pulse_ending_value,
                                 uint32_t *pulse_time_ticks)
                                 
{
    cmp_ctx_t ctx;

     msgpack_rx_handler_init(&ctx, data, data_len);
     if(  msgpack_rx_handler_find_unsigned(&ctx,"PULSE_PIN", pulse_pin) != true )
     {
         return false;
         
     }
     if(  msgpack_rx_handler_find_unsigned(&ctx,"PULSE_STARTING_VALUE", pulse_starting_value) != true )
     {
         return false;
         
     }
     if(  msgpack_rx_handler_find_unsigned(&ctx,"PULSE_ENDING_VALUE",pulse_ending_value) != true )
     {
         return false;
         
     }
     if(  msgpack_rx_handler_find_unsigned(&ctx,"PULSE_TIME_TICK",pulse_time_ticks) != true)
     {
         return false;
         
     }
     if( verifiy_output_pins(1,pulse_pin) == false){return false;}

     return true;
    
}
                                 