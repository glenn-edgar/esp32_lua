/*
** Build Time: 2018-10-03 17:05:58.409456  
*/
#ifndef _CF_ASSEMBLER_H_
#define _CF_ASSEMBLER_H_



#define CHAIN_NUM   3
#define LINK_NUM    16



#include "chain_flow_support.h" 
#include "cf_runtime_functions.h" 
#include "cf_external_functions.h" 
static char  chain_state[3];
static char  link_state[16];
static unsigned link_data[16];
static unsigned start_state[]={1,1,0};



static LINK_CELL CHAIN_initialization_LINKS[]= 
{
{ one_step_fn,(CF_FUN_AUX)analog_output_store_cf_handle_ref,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)analog_output_subscribe_configuration,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)analog_ouptut_subscribe_output,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)analog_output_subscribe_pulse_output,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)add_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ wait_condition_fn,(CF_FUN_AUX)wait_for_mqtt_connect,(unsigned)0,(unsigned)0,(unsigned)0},
{ enable_chain_fn,(CF_FUN_AUX)NULL,(unsigned)pulse_task,(unsigned)-1,(unsigned)-1},
{ terminate_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_feed_watch_dog_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_SECOND_TICK,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)pat_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_pulse_task_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_PULSE_EVENT,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)analog_output_set_initial_value,(unsigned)0,(unsigned)0,(unsigned)0},
{ wait_condition_fn,(CF_FUN_AUX)analog_output_step_timer,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)analog_output_set_final_value,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};



static CHAIN_LINK chain_control[] =
{
{ 0,0,8,CHAIN_initialization_LINKS},
{ 8,1,3,CHAIN_feed_watch_dog_LINKS},
{ 11,2,5,CHAIN_pulse_task_LINKS},
};
#endif
