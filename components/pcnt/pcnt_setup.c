
#include "driver/pcnt.h"
#include "pcnt_setup.h"

void pcnt_setup_counter_configure( int unit, int channel, int gpio_pin )
{
    
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = gpio_pin,
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,
        .channel = channel,
        .unit = unit,
        // What to do on the positive / negative edge of pulse input?
        .pos_mode = PCNT_COUNT_INC,   // Count up on the positive edge
        .neg_mode = PCNT_COUNT_INC,   // Keep the counter value on the negative edge
        // What to do when control input is low or high?
        .lctrl_mode = PCNT_MODE_KEEP, // Reverse counting direction if low
        .hctrl_mode = PCNT_MODE_KEEP,    // Keep the primary counter mode if high
        // Set the maximum and minimum limit values to watch
        .counter_h_lim = 0xffff,
        .counter_l_lim = 0,
    };
    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);
     /* Initialize PCNT's counter */
    pcnt_counter_pause(unit);
    pcnt_counter_clear(unit);
   
     
}

int pcnt_setup_read_and_resume(int unit)
{
    int16_t count;
    pcnt_counter_pause(unit);
    pcnt_get_counter_value(unit, &count);
    pcnt_counter_clear(unit);
    pcnt_counter_resume(unit);
    return (int)count;
}