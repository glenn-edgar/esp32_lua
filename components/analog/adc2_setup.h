#ifndef _ADC2_SETUP_H_
#define _ADC2_SETUP_H_

/*


ADC_ATTEN_DB_0 = 0
The input voltage of ADC will be reduced to about 1/1

ADC_ATTEN_DB_2_5 = 1
The input voltage of ADC will be reduced to about 1/1.34

ADC_ATTEN_DB_6 = 2
The input voltage of ADC will be reduced to about 1/2

ADC_ATTEN_DB_11 = 3
The input voltage of ADC will be reduced to about 1/3.6



enum adc2_channel_t
Values:

ADC2_CHANNEL_0 = 0
ADC2 channel 0 is GPIO4

ADC2_CHANNEL_1
ADC2 channel 1 is GPIO0

ADC2_CHANNEL_2
ADC2 channel 2 is GPIO2

ADC2_CHANNEL_3
ADC2 channel 3 is GPIO15

ADC2_CHANNEL_4
ADC2 channel 4 is GPIO13

ADC2_CHANNEL_5
ADC2 channel 5 is GPIO12

ADC2_CHANNEL_6
ADC2 channel 6 is GPIO14

ADC2_CHANNEL_7
ADC2 channel 7 is GPIO27

ADC2_CHANNEL_8
ADC2 channel 8 is GPIO25

ADC2_CHANNEL_9
ADC2 channel 9 is GPIO26

ADC2_CHANNEL_MAX
*/






#include "common_adc.h"

void adc2_channel_setup_channel(int channel, int attenuation);

int adc2_channel_make_measurement(int channel,int sample_number);

void adc2_setup_initialize_moving_average( ADC_MOVING_AVERAGE *moving_average,
                                      int                channel,
                                      float              r,
                                      int                measurement_number);

float adc2_moving_average_measurement(ADC_MOVING_AVERAGE *moving_average);

















#endif
