#ifndef _APP_INPUT_READ_CONFIG_H_
#define _APP_INPUT_READ_CONFIG_H_



bool app_output_read_file_configuration( void );


bool  app_output_find_set_configuration( uint32_t data_len, char *data);

bool  app_output_find_set_data( uint32_t data_len, char *data);
bool app_output_find_pulse_data( uint32_t data_len, char *data,
                                 uint32_t *pulse_pin,
                                 uint32_t *pulse_starting_value,
                                 uint32_t *pulse_ending_value,
                                 uint32_t *pulse_time_ticks);

#endif