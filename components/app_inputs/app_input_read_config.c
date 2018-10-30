
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "debounce_ctrl.h"
#include "gpio.h"
#include "msgpack_rx_handler.h"
#include "app_input_read_config.h"




bool app_input_file_read_configuration( uint32_t *input_number, uint32_t **pins,DEBOUNCE_CTRL **debounce_ctrl )
{
    
    
   
    char     *buffer;
    uint32_t buffer_size = 1000;
    cmp_ctx_t ctx;

    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_INPUT.MPK",  &buffer,&buffer_size ) != true)
    {
         //printf("############## cannot read file \n");
         return false;
    }
   
    app_input_read_setup( buffer_size, buffer, input_number, pins, debounce_ctrl );
    free(buffer);
    
    return false;    
    
}


bool app_input_read_setup( uint32_t data_len, char *data, uint32_t *input_number, uint32_t **pins, DEBOUNCE_CTRL **debounce_ctrl )
{
     uint32_t      pin_count;
    uint32_t    pullup_count;
    uint32_t    debounce_count;
    uint32_t *pin_array;
    uint32_t *pullup_array;
    DEBOUNCE_CTRL *debounce;
   
    
    cmp_ctx_t ctx;
    uint32_t *pin_ptr;
    uint32_t *pullup_ptr;
    
    msgpack_rx_handler_init(&ctx, data, data_len);
    if( msgpack_rx_handler_find_unsigned(&ctx,"debounce", &debounce_count )!= true)
    {
         goto exit;
    }
    if( msgpack_rx_handler_find_array_count(&ctx,"pins",&pin_count) != true )
    {
        goto exit;
    }
    if( msgpack_rx_handler_find_array_count(&ctx,"pullup",&pullup_count) != true )
    {
         goto exit;
    }
    if( pullup_count != pin_count )
    {
         goto exit;
    }
    *input_number = pin_count;
    if(pin_count >0 )
    {
      
      pin_array = malloc(pin_count*sizeof(uint32_t));
      pullup_array = malloc(pin_count *sizeof(uint32_t));
      debounce = malloc( pin_count*sizeof(DEBOUNCE_CTRL));
      *debounce_ctrl = debounce;
      *pins = pin_array;
      pin_ptr = pin_array;
      pullup_ptr = pullup_array;
      if( msgpack_rx_handler_find_array_unsigned(&ctx,"pins",&pin_count,pin_array) != true )
      {
           free(pin_array);
           free(pullup_array);
           free(debounce);
           goto exit;
      }
      if( msgpack_rx_handler_find_array_unsigned(&ctx,"pullup",&pullup_count,pullup_array) != true )
      {
          free(pin_array);
          free(pullup_array);
          free(debounce);
          goto exit;
      }
 
      for(int i=0;i<pin_count;i++)    
      {
           debounce_control_int( debounce, debounce_count );
           debounce++;
           gpio_configure_input(*pin_ptr,*pullup_ptr);
           pin_ptr++;
           pullup_ptr++;
           
      }
      free(pullup_array);
      
      return true;
     
      
    }

exit:
   
    return false;    
    
}

bool app_input_read_input(uint32_t data_len, 
                          char *data, 
                          uint32_t *input_number, 
                          uint32_t **pins )
{
    cmp_ctx_t ctx;
    msgpack_rx_handler_init(&ctx, data, data_len);
    if( msgpack_rx_handler_find_array_count(&ctx,"pins",input_number) != true )
    {
        return false;
    }
    *pins = malloc(*input_number*sizeof(uint32_t));
    if( msgpack_rx_handler_find_array_unsigned(&ctx,"pins",input_number,*pins) != true )
    {
        return false;
    }
    return true;
}
