/*
**
** File cf_status_bit.c
**
**
**
*/
#include <esp_types.h>

#include "chain_flow_support.h"
#include "cf_status_bit.h"






void cf_initialize_status_manager( CHAIN_FLOW_HANDLE *cf )
{
   cf->cf_status_register = 0;
}

void  cf_status_clear_bit( CHAIN_FLOW_HANDLE *cf,uint32_t status_bit)
{
 

    cf->cf_status_register = cf->cf_status_register &(~status_bit);
    
}

void cf_clear_status(CHAIN_FLOW_HANDLE *cf )
{
      
  

    cf->cf_status_register = 0;
    
}

void cf_set_interrupt_status_bit( CHAIN_FLOW_HANDLE *cf,uint32_t status_bit )
{
     cf-> cf_status_register |= status_bit;

}

void cf_set_status_bit( CHAIN_FLOW_HANDLE *cf,uint32_t status_bit )
{
    
    
   

   cf->cf_status_register |= status_bit;
   
}


uint32_t cf_get_status(CHAIN_FLOW_HANDLE *cf )
{
   int return_value;

   return_value = cf->cf_status_register;
   cf->cf_status_register = 0;
   
   return return_value;
}
             
  



