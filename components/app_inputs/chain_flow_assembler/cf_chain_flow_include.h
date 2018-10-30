/*
** Build Time: 2018-10-04 09:00:44.853985  
*/
#ifndef _CF_ASSEMBLER_H_
#define _CF_ASSEMBLER_H_



#define CHAIN_NUM   3
#define LINK_NUM    12



#include "chain_flow_support.h" 
#include "cf_runtime_functions.h" 
#include "cf_external_functions.h" 
static char  chain_state[3];
static char  link_state[12];
static unsigned link_data[12];
static unsigned start_state[]={1,1,0};



static LINK_CELL CHAIN_initialization_LINKS[]= 
{
{ one_step_fn,(CF_FUN_AUX)add_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)app_input_register_configure_read,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)app_input_register_read,(unsigned)0,(unsigned)0,(unsigned)0},
{ wait_condition_fn,(CF_FUN_AUX)wait_for_mqtt_connect,(unsigned)0,(unsigned)0,(unsigned)0},
{ enable_chain_fn,(CF_FUN_AUX)NULL,(unsigned)sampling_task,(unsigned)-1,(unsigned)-1},
{ terminate_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_feed_watch_dog_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_SECOND_TICK,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)pat_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_sampling_task_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_TIME_TICK_EVENT,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)app_input_sample_inputs,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};



static CHAIN_LINK chain_control[] =
{
{ 0,0,6,CHAIN_initialization_LINKS},
{ 6,1,3,CHAIN_feed_watch_dog_LINKS},
{ 9,2,3,CHAIN_sampling_task_LINKS},
};
#endif
