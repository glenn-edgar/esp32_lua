
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include <esp_types.h>
#include "freertos/semphr.h"

#include "console_output_server.h"


#include "hexadecimal.h"
#include "crc-16.h"

static SemaphoreHandle_t xSemaphore = NULL;

bool console_output_server(void)
{
     
     vSemaphoreCreateBinary( xSemaphore );
     return true;
    
}

bool console_output(const char *format, ...)
{
    bool return_value;
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
       
        va_list args;
        va_start(args, format);
        return_value = vprintf(format, args);
        va_end(args);
        xSemaphoreGive( xSemaphore );
    }
    else
    {
        return_value = false;
    }
    return return_value;
    
}


bool console_output_structured_data(int number, MSG_PACK_ELEMENT *msg_pack)
{
    bool return_value;
    char *pack_buffer;
    int   pack_buffer_size;
  
  
   
    
    pack_buffer = msg_dict_stream( &pack_buffer_size,number,msg_pack);
    
    return_value = console_output_binary_data("MSGPACK:",pack_buffer,pack_buffer_size);
    

    free(pack_buffer);
    
    
    return return_value;
       
    
    
}

bool console_output_binary_data(char *prefix,char *binary_data,int binary_size)
{
    bool return_value;
     char *master_buffer;
    char crc_buf[6];
    uint16_t crc16;

     memset(crc_buf,0,sizeof(crc_buf)-1);
    crc16 = calc_crc16(binary_data, binary_size);

    binary_to_hex((char*)&crc16,sizeof(crc16),crc_buf);

    master_buffer = malloc(binary_size*2+50);
    memset(master_buffer,0,binary_size*2+50);
    strcpy(master_buffer,prefix);
    
    binary_to_hex(binary_data,binary_size, master_buffer+strlen("MSGPACK:"));
    strcat(master_buffer,crc_buf);
 
    strcat(master_buffer,":END");
    
 
 
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
        
         puts(master_buffer);
         xSemaphoreGive( xSemaphore );
    }
    else
    {
        return_value = false;
    }
    
    free(master_buffer);
    
    return return_value;   
    
    
    
}
