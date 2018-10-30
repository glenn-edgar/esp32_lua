/*
** File cb_status_bit.h
** File to dispatch actions based upon status bits.
** 
**
**
**
*/

#ifndef CF_STATUS_BIT_H_
#define CF_STATUS_BIT_H
#include "chain_flow_support.h"   
/*
Status Bit Definitions
Max 32 bits
*/

    
//define CF_PROCESS_TIMER_TICK    1
//define CF_PROCESS_MODBUS       2
// next bit would be 4
// next bit would be 8
    
  
    
void cf_initialize_status_manager( CHAIN_FLOW_HANDLE *cf );
void cf_set_status_bit( CHAIN_FLOW_HANDLE *cf,uint32_t status_bit );
uint32_t cf_get_status( CHAIN_FLOW_HANDLE *cf );
void cf_clear_status(CHAIN_FLOW_HANDLE *cf );
void  cf_status_clear_bit( CHAIN_FLOW_HANDLE *cf, uint32_t status_bit);
    
#endif