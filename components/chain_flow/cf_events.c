/*
**
** File: cf_events.c
**
**
**
**
*/
#include <esp_types.h>
#include "chain_flow_support.h"
#include "cf_events.h"






void cf_initialize_event_manager(  CHAIN_FLOW_HANDLE *cf )
{
   cf->rx_index     = 0;
   cf->tx_index     = 0;
   cf->event_number = 0;
}



int cf_event_number( CHAIN_FLOW_HANDLE *cf)
{
    
    return cf->event_number;
    
}


void cf_send_event(  CHAIN_FLOW_HANDLE *cf,unsigned event, unsigned data )
{
  
   if( cf->event_number < EVENT_SIZE )
   {
     cf->event_data[cf->rx_index].event      = event;
     cf->event_data[cf->rx_index].data       = data;
     cf->event_number++;
     cf->rx_index = ( cf->rx_index +1 )&EVENT_MASK;
   }
  
}


int cf_rx_event(  CHAIN_FLOW_HANDLE *cf,unsigned *event, unsigned *data )
{
   int return_value;

   
   if( cf->event_number > 0 )
   {
      *event          = cf->event_data[cf->tx_index].event;
      *data     = cf->event_data[cf->tx_index].data;
      return_value    = 1;
      cf->event_number--;
      cf->tx_index = ( cf->tx_index +1 )&EVENT_MASK;

   }
   else
   {
     return_value = 0;
   }
  
   return return_value;
}
             
  
