#ifndef _GPIO_H_
#define _GPIO_H_





void gpio_config_output_pin( int pin );
void gpio_config_input_pin( int pin );
void gpio_config_pull_up( int pin );
void gpio_config_pull_down( int pin );
void gpio_set_value(int pin, int value );
int gpio_read_value(int pin);
void gpio_configure_input(int pin,int pull_up_mode);


#endif


