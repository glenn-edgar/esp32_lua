
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_vfs_fat.h"
#include "esp_task_wdt.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "watchdog.h"
#include "console_output_server.h"
#include "console_input_server.h"
#include "message_processor.h"
#include "hexadecimal.h"
#include "crc-16.h"

#define INPUT_BUFFER_SIZE 4000

static int input_index;
static char input_buffer[INPUT_BUFFER_SIZE];
static char binary_buffer[INPUT_BUFFER_SIZE/2];
static TaskHandle_t xHandle = NULL;
static void console_input_task( void * pvParameters );
static void init_input_buffer(void);
static bool build_buffer(int *input_number);
void initialize_console_input_server( void)
{
    
  xTaskCreate( console_input_task, "CONSOLE_INPUT_PROCESSOR",4000, NULL, 10, &xHandle );
    configASSERT( xHandle );

    
}

// Task to be created.
static void console_input_task( void * pvParameters )
{
 int crc_16;
 int msg_pack_number;
 
 uint16_t *msg_crc;
 MSG_PACK_ELEMENT *msg_pack_buffer;
 int binary_buffer_size;
 int input_number;
 wdt_task_subscribe();
 init_input_buffer();
 for( ;; )
 {
   wdt_reset_task_time();
   
  
   if( build_buffer(&input_number)  == true )
   { 
    //printf("build buffer is true \n");
    binary_buffer_size = input_number/2;    
    hex_to_binary(input_buffer,input_number, binary_buffer);   
    crc_16 = calc_crc16(binary_buffer,binary_buffer_size-2);
    //printf("input data --->%*s\n",input_number,(char *)input_buffer);
    //printf("input_length %d\n",input_number);
    msg_crc = (uint16_t *)&binary_buffer[binary_buffer_size-2];
    //printf("CRC %d %d \n",*msg_crc,crc_16);
    if( crc_16 == *msg_crc)
    {
        //printf("crc match \n");
        
    

     
     if( message_processor(&msg_pack_number,&msg_pack_buffer,
                        binary_buffer,binary_buffer_size) == true)
     {       
              
         console_output_structured_data(msg_pack_number,msg_pack_buffer);
         
     }
     else
     {
       // printf("no message processor match\n");
     }
     if(msg_pack_number >0)
     {
         free(msg_pack_buffer);
     }
    } 
    else
    {
       printf("********************** crc no match -------------------------\n");
    }    
     init_input_buffer();
   }
   vTaskDelay(10/ portTICK_PERIOD_MS);

 }
}

static void init_input_buffer(void)
{
    input_index = 0;
}

static bool build_buffer(int *input_number)
{
    
    int  temp;
    
    temp = getchar();
    
    while(temp>=0)
    {
        
        input_buffer[input_index] = temp;
        
        input_index += 1;
        if( input_index >= sizeof(input_buffer))
        {
           input_index = 0;
           return false;           
        }
        if( temp == '\n')
        {
            input_index -= 1;
            input_buffer[input_index]=0;
            *input_number = input_index;
             if(input_index < 6 )
            {
                return false;
                
            }
           
             return true;
        }
        temp = getchar();
    }
    return false;
    
  
}