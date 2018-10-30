#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "esp_system.h"
#include "esp_adc_cal.h"

#include "adc2_setup.h"


void adc2_channel_setup_channel(int channel, int attenuation)
{
    adc2_config_channel_atten(channel, attenuation);
}

int adc2_channel_make_measurement(int channel, int sample_number)
{
    int read_raw;
    int total = 0;
    for(int i = 0;i<sample_number;i++)
    {
         adc2_get_raw( channel, ADC_WIDTH_12Bit, &read_raw);
         total += read_raw;
        
    }
    return (total+(sample_number/2))/sample_number;
    
}

void adc2_setup_initialize_moving_average( ADC_MOVING_AVERAGE *moving_average,
                                      int                channel,
                                      float              r,
                                      int                measurement_number)
{
   moving_average->channel = channel;
   moving_average->r = r;
   moving_average->measurement_number = measurement_number;
   moving_average->result = adc2_channel_make_measurement(channel,measurement_number);
    
    
}

float adc2_moving_average_measurement(ADC_MOVING_AVERAGE *ma)
{
    float temp1;
    
    temp1 = adc2_channel_make_measurement(ma->channel, ma->measurement_number);
    ma->result = (temp1*(1-ma->r)) +(ma->result*ma->r);
    return ma->result;
}
    
    
    


