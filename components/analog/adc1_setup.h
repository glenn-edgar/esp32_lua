#ifndef _ADC1_SETUP_H_
#define _ADC1_SETUP_H_
/*

Enumerations
enum adc_atten_t
Values:

ADC_ATTEN_DB_0 = 0
The input voltage of ADC will be reduced to about 1/1

ADC_ATTEN_DB_2_5 = 1
The input voltage of ADC will be reduced to about 1/1.34

ADC_ATTEN_DB_6 = 2
The input voltage of ADC will be reduced to about 1/2

ADC_ATTEN_DB_11 = 3
The input voltage of ADC will be reduced to about 1/3.6

ADC_ATTEN_MAX
enum adc_bits_width_t
Values:



ADC_WIDTH_MAX
enum adc1_channel_t
Values:

ADC1_CHANNEL_0 = 0
ADC1 channel 0 is GPIO36

ADC1_CHANNEL_1
ADC1 channel 1 is GPIO37

ADC1_CHANNEL_2
ADC1 channel 2 is GPIO38

ADC1_CHANNEL_3
ADC1 channel 3 is GPIO39

ADC1_CHANNEL_4
ADC1 channel 4 is GPIO32

ADC1_CHANNEL_5
ADC1 channel 5 is GPIO33

ADC1_CHANNEL_6
ADC1 channel 6 is GPIO34

ADC1_CHANNEL_7
ADC1 channel 7 is GPIO35

ADC1_CHANNEL_MAX

*/



#include "common_adc.h"

void adc1_channel_setup_channel(int channel, int attenuation);

int adc1_channel_make_measurement(int channel, int sample_number);

void adc1_setup_initialize_moving_average( ADC_MOVING_AVERAGE *moving_average,
                                      int                channel,
                                      float              r,
                                      int                measurement_number);

float adc1_moving_average_measurement(ADC_MOVING_AVERAGE *moving_average);


#endif