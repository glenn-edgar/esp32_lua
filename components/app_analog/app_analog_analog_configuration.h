#ifndef _APP_ANALOG_ANALOG_CONFIG_H_
#define _APP_ANALOG_ANALOG_CONFIG_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_types.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

typedef struct
{
    esp_adc_cal_characteristics_t *adc_chars;
    adc1_channel_t   adc_channel;
    adc_atten_t   attenuation;
       
}AD1_CONTROL_T;

bool app_analog_find_analog_configuration_data(void);

uint32_t app_analog_get_analog_data( AD1_CONTROL_T **ad1_pointer);

#endif