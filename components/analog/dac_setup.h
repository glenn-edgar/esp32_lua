#ifndef _DAC_SETUP_H_
#define _DAC_SETUP_H_


/*
API Reference
Header Files
components/driver/include/driver/dac.h
Enumerations
enum dac_channel_t
Values:

DAC_CHANNEL_1 = 1
DAC channel 1 is GPIO25

DAC_CHANNEL_2
DAC channel 2 is GPIO26

DAC_CHANNEL_MAX
Functions
esp_err_t dac_out_voltage(dac_channel_tchannel, uint8_t dac_value)
Set DAC output voltage.

DAC output is 8-bit. Maximum (255) corresponds to VDD.

Note
When this function is called, function for the DAC channel’s GPIO pin is reconfigured for RTC DAC function.
Return
ESP_OK success
ESP_ERR_INVALID_ARG Parameter error
Parameters
channel: DAC channel
dac_value: DAC output value
*/

#include "driver/dac.h"

#endif