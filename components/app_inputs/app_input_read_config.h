#ifndef _APP_INPUT_READ_CONFIG_H_
#define _APP_INPUT_READ_CONFIG_H_

#include "debounce_ctrl.h"
bool app_input_file_read_configuration( uint32_t *input_number, uint32_t **pins,DEBOUNCE_CTRL **debounce_ctrl );
bool app_input_read_input(uint32_t data_len, char *data, uint32_t *input_number, uint32_t **pins );
bool app_input_read_setup( uint32_t data_len, char *data, uint32_t *input_number, uint32_t **pins,DEBOUNCE_CTRL **debounce_ctrl );

#endif