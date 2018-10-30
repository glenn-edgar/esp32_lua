
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include <esp_types.h>
#include "freertos/semphr.h"

#include "msg_command_data.h"

#include "hexadecimal.h"
#include "message_processor.h"
#include "msg_process_builtin_cmds.h"

static SemaphoreHandle_t xSemaphore = NULL;



void init_message_processor(void)
{
    vSemaphoreCreateBinary( xSemaphore );
    msg_command_data_initialize();
    
}

bool message_processor(int *msg_pack_number, 
                       MSG_PACK_ELEMENT **msg_pack,
                       char * input_buffer,int input_number)
{
    bool return_value;
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
        return_value =msg_command_process_packet(msg_pack_number,msg_pack,input_buffer,input_number);
        xSemaphoreGive( xSemaphore );
    }
    else
    {
        return_value = false;
    }
    return return_value;

    
}

