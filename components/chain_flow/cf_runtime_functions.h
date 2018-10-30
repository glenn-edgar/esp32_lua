/*

  File: cf_runtime_functions.h


*/

#ifndef _CF_RUNTIME_FUNCTIONS_H_
#define _CF_RUNTIME_FUNCTIONS_H_



#include "chain_flow_support.h"
    
int terminate_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int halt_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int one_step_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int code_step_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int reset_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int send_event_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int wait_tod_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int wait_event_count_fn(CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);


int wait_event_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int wait_time_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int wait_condition_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int verify_condition_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int verify_not_timeout_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int verify_not_event_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int verify_not_tod_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int nop_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int enable_chain_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int disable_chain_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int change_state_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int system_reset_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int suspend_chain_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);

int resume_chain_fn( CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);







#endif
