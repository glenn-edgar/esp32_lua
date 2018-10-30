#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include <esp_types.h>
#include "freertos/semphr.h"
#include "status_interface.h"


static bool init_flag = false;
static uint32_t current_status= 0;
static SemaphoreHandle_t xSemaphore = NULL;


void init_status_interface(void)
{
    
    if(init_flag == false)
    {
        vSemaphoreCreateBinary( xSemaphore );
        init_flag = true;
        current_status = 0;
    }
    
    
    
}




void change_status_value(uint32_t mask, uint32_t value)
{

    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
       
        
        current_status = (current_status & (~mask))|(value&mask);
        xSemaphoreGive( xSemaphore );
    }
  
    
    
    
}



uint32_t get_status_value(void)
{
    uint32_t return_value;
    return_value = 0;
    
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
       
        
        return_value = current_status;
        xSemaphoreGive( xSemaphore );
    }
    return return_value;
}

