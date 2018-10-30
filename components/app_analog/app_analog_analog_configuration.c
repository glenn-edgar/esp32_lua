#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmp.h"
#include "msgpack_rx_handler.h"
#include "app_analog_analog_configuration.h"


#define DEFAULT_VREF    1100

static uint32_t ad1_channel_number;
static AD1_CONTROL_T ad1_control[ADC1_CHANNEL_MAX];


static bool app_analog_find_analog_configuration( uint32_t  buffer_size, char *buffer);


uint32_t app_analog_get_analog_data( AD1_CONTROL_T **ad1_pointer)
{
    *ad1_pointer = ad1_control;
    return ad1_channel_number;
}


bool app_analog_find_analog_configuration_data(void) 
{
    bool          return_value;
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
       
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_ADC1.MPK" ,  &buffer,&buffer_size ) != true)
    {
         
         return false;
    }
   
    
    return_value = app_analog_find_analog_configuration(   buffer_size, buffer);
    
    free(buffer);
    return return_value;    
    
}


static bool app_analog_process_buffer_a( uint32_t index, uint32_t data_len , char *data )
{
   cmp_ctx_t ctx;
   uint32_t  attenuation;
   uint32_t  channel;
   esp_adc_cal_characteristics_t *adc_chars;
   
   msgpack_rx_handler_init(&ctx, data, data_len); 
   
   
   if( msgpack_rx_handler_find_unsigned(&ctx,"attenuation", &attenuation ) == false)
   {
       printf("attenuation failed \n");
       return false;
       
   }  
    
   
   
   if( msgpack_rx_handler_find_unsigned(&ctx,"channel", &channel ) == false)
   {
       printf("channel failed \n");
       return false;
       
   }  
   
   adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
   esp_adc_cal_characterize(ADC_UNIT_1, attenuation, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
   ad1_control[index].adc_chars =adc_chars;
   ad1_control[index].adc_channel = channel;
   ad1_control[index].attenuation = attenuation;
   adc1_config_channel_atten(channel, attenuation);
   return true;
    
}



static bool  app_analog_find_analog_configuration(  uint32_t data_len, char *data )
{ 

    uint32_t binary_length[ ADC1_CHANNEL_MAX ];
    char   *binary_buffer[ ADC1_CHANNEL_MAX];

    cmp_ctx_t ctx;
    uint32_t adc_count;
    
    msgpack_rx_handler_init(&ctx, data, data_len);
    
    if( msgpack_rx_handler_find_array_count(&ctx,"ANALOG_CHANNELS",&adc_count) != true )
    {
        return false;
    }
    
    if(adc_count >= ADC1_CHANNEL_MAX){ return false; }
    ad1_channel_number = adc_count;
    
   
    if(msgpack_rx_handler_get_binary_array(&ctx,"ANALOG_CHANNELS",&adc_count,binary_length,binary_buffer) == false)
    {
       
        return false;
    }
    adc1_config_width(ADC_WIDTH_BIT_12);
    for(int i = 0; i< adc_count ;i++)
    {
       
       if( app_analog_process_buffer_a(i, binary_length[i],binary_buffer[i]) == false)
       {
           
           return false;
       }
       
    }

   
    return true;       
    
    
}


