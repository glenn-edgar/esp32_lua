#ifndef _APP_ANALOG_EVENT_QUEUE_H_
#define _APP_ANALOG_EVENT_QUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "driver/gpio.h"
#include "driver/adc.h"


#define ANALOG_CHANNEL_NUMBER 6




typedef struct
{
    uint32_t ad_channel;
    float dc_average;
    float sd_average;
}APP_ANALOG_MQTT_CHANNEL;

typedef struct
{
    
    uint32_t                 channel_number;
    APP_ANALOG_MQTT_CHANNEL  app_analog_mqtt_channel[ANALOG_CHANNEL_NUMBER];
}APP_MQTT_STRUCTURE;




#endif