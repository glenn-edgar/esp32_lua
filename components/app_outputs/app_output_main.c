

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "esp_event.h"
#include "freertos/task.h"
#include "cf_events.h"
#include "cf_status_bit.h"
#include "app_output_read_config.h"
#include "app_output_functions.h"
#include "chain_flow_support.h"
#include "app_output_local_events.h"
#include "chain_flow_assembler/cf_chain_flow_include.h"

#include "app_output_main.h"

static TaskHandle_t xHandle = NULL;
static void app_output_task( void * pvParameters );
static void load_chain_flow_data( CHAIN_FLOW_HANDLE *cf );

void initialize_app_output_main(void)
{
       if( app_output_read_file_configuration(  ) == true)
       {
         xTaskCreate( app_output_task, "APP_OUTPUT_TASK",4000, NULL, 20, &xHandle );
         configASSERT( xHandle );
       }
   
    
}
static inline void  process_status_data( CHAIN_FLOW_HANDLE *cf, unsigned status_data)
{

;
    
}

// Task to be created.
static void app_output_task( void * pvParameters )
{
    int second_sub_count;
    int minute_sub_count;
    uint32_t status_data;
    uint32_t event_number;
    unsigned event_id;
    unsigned event_data;
    CHAIN_FLOW_HANDLE cf;
    
    minute_sub_count = 0;
    second_sub_count = 0;
    
    
    load_chain_flow_data( &cf );
    
    initialize_cf_system(&cf);

    for(;;)
    {
        while( cf_event_number(&cf) != 0)
        {
           status_data = cf_get_status(&cf);
           if( status_data != 0 )
           {
               process_status_data(&cf, status_data);
             
           }
           else
           {
           
             event_number = cf_rx_event( &cf, &event_id, &event_data );
             if( event_number > 0 )
             {
              
              cf_process_cf_event( &cf,event_id, event_data);
             }
           }
        }
        vTaskDelay(10/ portTICK_PERIOD_MS); 
        cf_send_event( &cf,CF_TIME_TICK_EVENT, 10 );
        second_sub_count += 10;
        if(second_sub_count >= 1000)
        {
          second_sub_count = 0;
          
          cf_send_event( &cf,CF_SECOND_TICK, 1 );
          minute_sub_count += 1;
          if(minute_sub_count >= 60 )
          {
              cf_send_event( &cf,CF_MINUTE_TICK,1 );
              
              minute_sub_count = 0;
          }
            
        }
 
       
    }       
      
    
}

static void load_chain_flow_data( CHAIN_FLOW_HANDLE *cf )
{
    memset(cf,0,sizeof(CHAIN_FLOW_HANDLE));
 
    cf->number_of_chains =CHAIN_NUM;
    cf->number_of_links = LINK_NUM;
    cf->chain_state = chain_state;
    cf->link_state = link_state;
    cf->link_data  = link_data;
    cf->start_state = start_state;
    cf->chain_control = chain_control;

}