#ifndef _DEBOUNCE_CTRL_H_
#define _DEBOUNCE_CTRL_H_


typedef struct
{
    unsigned current_level;
    unsigned reference_level;
    unsigned current_count;
    unsigned transition_count;
}DEBOUNCE_CTRL;


void debounce_control_int( DEBOUNCE_CTRL *debounce_ctrl,unsigned transition_number );

bool debounce_control_update( DEBOUNCE_CTRL *debounce_ctrl, unsigned new_value);

#endif
