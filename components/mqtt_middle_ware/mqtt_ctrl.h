#ifndef _MQTT_CTRL_H_


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

typedef enum
{
    LOAD_CONFIGURATION   = 0,
    WAIT_FOR_WIFI_CONNECT,
    SETUP_MQTT_CONNECTION,
    WAIT_FOR_MQTT_CONNECT,
    RESTORE_SUBSCRIPTIONS,
    MQTT_OPERATIONAL_STATE,
    MQTT_ERROR_STATE,
    
} MQTT_STATE;


void mqtt_ctl_change_state( MQTT_STATE new_state);


MQTT_STATE mqtt_ctl_get_state(void);

void task_wait_for_mqtt_connection(void);


void initialize_mqtt_client_handler( void);

bool mqtt_clt_subscribe(char *ref_topic);
bool mqtt_clt_publish(char *ref_topic, char *data, int data_length );

#endif