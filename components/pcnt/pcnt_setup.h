#ifndef _PCNT_SETUP_H_
#define _PCNT_SETUP_H_



void pcnt_setup_counter_configure( int unit, int channel, int gpio_pin );

int pcnt_setup_read_and_resume(int unit);

#endif