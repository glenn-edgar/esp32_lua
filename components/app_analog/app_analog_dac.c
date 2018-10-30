/*
DAC values vs voltage output

dac         actual      expected    percent 
value		value		value		error
0xff		3.18		3.18		0
0xf0		3		2.98125		0.628930818
0xe0		2.81		2.7825		0.988319856
0xd0		2.61		2.58375		1.015965167
0xc0		2.42		2.385		1.467505241
0xb0		2.22		2.18625		1.54373928
0xa0		2.03		1.9875		2.13836478
0x90		1.835		1.78875		2.585604472
0x80		1.63		1.59		2.51572327
0x70		1.415		1.39125		1.707097934
0x60		1.221		1.1925		2.389937107
0x50		1.016		0.99375		2.238993711
0x40		0.823		0.795		3.522012579
0x30		0.629		0.59625		5.492662474
0x20		0.435		0.3975		9.433962264
0x10		0.232		0.19875		16.72955975
0x00		0.043				



*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_types.h"
#include "driver/gpio.h"
#include "driver/dac.h"
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "msgpack_rx_handler.h"
#include "msg_dict_stream.h"
#include "app_analog_dac.h"

typedef struct
{
    dac_channel_t dac_channel;
    uint8_t       dac_value;
    
    
}DAC_CONTROL_T;

static uint32_t dac_channel_number;
static DAC_CONTROL_T dac_control[DAC_CHANNEL_MAX];


static bool read_dac_file_configuration(void);
static void app_dac_change_output( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  );
    

static void app_dac_read_output( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  );
                          
static bool  app_dac_find_dac_configuration(  uint32_t data_len, char *data,bool setup_flag);

static bool app_dac_process_buffer_a(uint32_t index,uint32_t data_len , char *data,bool setup_flag);
                          
                      

void app_analog_setup_dacs(void ) // these are only subscription 
{
    
    if( read_dac_file_configuration() == true)
    {
       mqtt_ctrl_register_subscription("OUTPUTS/DAC/CHANGE_OUTPUT", app_dac_change_output );
       mqtt_ctrl_register_subscription("OUTPUTS/DAC/REQUEST_OUTPUT", app_dac_read_output );
    }
    else
    {
        printf("*********************dac file not found \n");
        
    }    
    
    
}


static bool read_dac_file_configuration( void ) 
{
    bool          return_value;
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
       
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_DAC.MPK" ,  &buffer,&buffer_size ) != true)
    {
        
         return false;
    }
   
    
    return_value = app_dac_find_dac_configuration(   buffer_size, buffer,true);
    
    free(buffer);
    return return_value;    
    
}



static bool app_dac_process_buffer_a( uint32_t dac_number, uint32_t data_len , char *data, bool setup_flag)
{
   cmp_ctx_t ctx;
   uint32_t  value;
   uint32_t  channel;
   uint32_t  index;
   
   msgpack_rx_handler_init(&ctx, data, data_len); 
   
   
   if( msgpack_rx_handler_find_unsigned(&ctx,"value", &value ) == false)
   {
       return false;
       
   }  
    
   
   if( setup_flag == true)
   {
     if( msgpack_rx_handler_find_unsigned(&ctx,"channel", &channel ) == false)
     {
       return false;
       
     }  
   }
   else
   {
     if( msgpack_rx_handler_find_unsigned(&ctx,"index", &index ) == false)
     {
       return false;
       
     }        
       
   }       
   
   if(setup_flag == true)
   {
     if( channel <  DAC_CHANNEL_1){ return false; }
     if( channel >=   DAC_CHANNEL_MAX){ return false; }
     dac_control[dac_number].dac_channel = channel;
     dac_control[dac_number].dac_value = value;
     dac_output_enable(channel);
     dac_output_voltage(channel,value);
   }
   else
   {
       if(index >= dac_channel_number){ return false;}
       dac_control[dac_number].dac_value = value;
       dac_output_voltage(dac_control[dac_number].dac_channel, value);
   }    
   return true;
    
}



static bool  app_dac_find_dac_configuration(  uint32_t data_len, char *data, bool setup_flag )
{ 

    uint32_t binary_length[ DAC_CHANNEL_MAX ];
    char   *binary_buffer[ DAC_CHANNEL_MAX];

    cmp_ctx_t ctx;
    uint32_t dac_count;
    
    msgpack_rx_handler_init(&ctx, data, data_len);
    
    if( msgpack_rx_handler_find_array_count(&ctx,"DAC_OUTPUTS",&dac_count) != true )
    {
        return false;
    }
   
    if(dac_count >= DAC_CHANNEL_MAX){ return false; }
    dac_channel_number = dac_count;
    
   
    if(msgpack_rx_handler_get_binary_array(&ctx,"DAC_OUTPUTS",&dac_count,binary_length,binary_buffer) == false)
    {
       
        return false;
    }

    for(int i = 0; i< dac_count ;i++)
    {
       
       if( app_dac_process_buffer_a(i, binary_length[i],binary_buffer[i],setup_flag) == false)
       {
           
           return false;
       }
       
    }

   
    return true;       
    
    
}




static void app_dac_change_output( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  )
{
    app_dac_find_dac_configuration(  data_len, data,false);
    
}                          
    

static void app_dac_read_output( esp_mqtt_client_handle_t mqtt_client,
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
   
 
   msg_pack_number = 3*dac_channel_number+3;
   msg_pack_number +=4; //safety
   pack_index = 0;
   
   msg_pack = malloc(msg_pack_number*sizeof(MSG_PACK_ELEMENT));
   
   msg_dict_pack_string(&msg_pack[pack_index++],"TOPIC","OUTPUTS/DAC/READ_OUTPUT"); 
   
  
   msg_dict_pack_array(&msg_pack[pack_index++],"DATA",dac_channel_number);
   for(  int i = 0;i<dac_channel_number;i++)
   {
      msg_dict_pack_map(&msg_pack[pack_index++],NULL, 2);

      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"CHANNEL", dac_control[i].dac_channel);
      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"VALUE", dac_control[i].dac_value);


      
       
   }
   
 
   pack_buffer = msg_dict_stream( &pack_buffer_size,pack_index,msg_pack);
   
   mqtt_clt_publish("OUTPUTS/DAC/READ_OUTPUT", pack_buffer,pack_buffer_size );
   free(pack_buffer);
   free(msg_pack);
 
    
    
}                  
