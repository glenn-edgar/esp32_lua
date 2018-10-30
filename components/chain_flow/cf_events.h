/*
**
** File cf_events.h
**
*/

#ifndef _CF_EVENTS_H_
#define _CF_EVENTS_H_

#define TICK_INTERVAL          10

#define CF_SYSTEM_INIT         0
#define CF_INIT_EVENT          1
#define CF_TIME_TICK_EVENT     2
#define CF_SECOND_TICK         5
#define CF_MINUTE_TICK         6
#define CF_APP_EVENT_START    10

#include "chain_flow_support.h"

void cf_initialize_event_manager( CHAIN_FLOW_HANDLE *cf );
int cf_event_number(CHAIN_FLOW_HANDLE *cf);

void cf_send_event( CHAIN_FLOW_HANDLE *cf, unsigned event, unsigned data );
int cf_rx_event( CHAIN_FLOW_HANDLE *cf, unsigned *event, unsigned *event_data );

#endif
