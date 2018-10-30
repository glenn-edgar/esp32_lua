#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "esp_system.h"
#include "esp_adc_cal.h"

#include "adc1_setup.h"
#include "adc2_setup.h"

void adc1_channel_setup_channel(int channel, int attenuation)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, attenuation);
}

int adc1_channel_make_measurement(int channel, int sample_number)
{
    int total = 0;
    for(int i = 0;i<sample_number;i++)
    {
         total += adc1_get_raw( channel);
        
    }
    return (total+(sample_number/2))/sample_number;
    
}

void adc1_setup_initialize_moving_average( ADC_MOVING_AVERAGE *moving_average,
                                      int                channel,
                                      float              r,
                                      int                measurement_number)
{
   moving_average->channel = channel;
   moving_average->r = r;
   moving_average->measurement_number = measurement_number;
   moving_average->result = adc1_channel_make_measurement(channel,measurement_number);
    
    
}

float adc1_moving_average_measurement(ADC_MOVING_AVERAGE *ma)
{
    float temp1;
    
    temp1 = adc1_channel_make_measurement(ma->channel, ma->measurement_number);
    ma->result = (temp1*(1-ma->r)) +(ma->result*ma->r);
    return ma->result;
}
    
    
    


