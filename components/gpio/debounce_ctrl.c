
#include <esp_types.h>
#include <string.h>
#include "debounce_ctrl.h"



void debounce_control_int( DEBOUNCE_CTRL *debounce_ctrl,unsigned transition_number )
{
    
    memset(debounce_ctrl,0,sizeof(DEBOUNCE_CTRL));
    debounce_ctrl->transition_count = transition_number;
    
    
}

bool debounce_control_update( DEBOUNCE_CTRL *debounce_ctrl, unsigned new_value)
{
    bool return_value;
    return_value = false;
    if( new_value == debounce_ctrl->current_level )
    {
        if(debounce_ctrl->transition_count > debounce_ctrl->current_count)
        {
            debounce_ctrl->current_count +=1;
            if(debounce_ctrl->transition_count == debounce_ctrl->current_count)
            {
                return_value = true;
                debounce_ctrl->reference_level = debounce_ctrl->current_level;
            }
        }
        
        
    }
    else
    {
       debounce_ctrl->current_count = 0;
       debounce_ctrl->current_level = new_value;       
    }
    return return_value;    
}
    
    
    
    


