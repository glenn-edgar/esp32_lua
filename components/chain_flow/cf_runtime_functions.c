/*

File: cf_runtime_functions.c
Implements of codes for cf_runtime_functions.c


*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <esp_types.h>
#include "chain_flow_support.h"

#include "cf_runtime_functions.h"
#include "cf_events.h"

#define NULL_PARAMETER  0xffffffff

int terminate_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   return CF_TERMINATE;
}


int halt_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{

  return CF_HALT;
}


 
int reset_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   
   return CF_RESET;

}

int one_step_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   int return_value;   
   
   if( event!= CF_INIT_EVENT )
   {
      param_1(cf,link_id, param_2,param_3,param_4,event,event_data );
      return_value = CF_DISABLE;
   }
   else
   {
      return_value = CF_CONTINUE;
   }
   return return_value; 
}

int code_step_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   
   return param_1( cf,link_id, param_2,param_3,param_4,event,event_data );

}


int send_event_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
 
   if( event== CF_INIT_EVENT )
   {
      cf_send_event(cf, param_2, param_3 );
   }
   return CF_DISABLE;
}


int wait_tod_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   
#if 0
   uint32_t time;
   time = RTC_1_GetTime();
 
   // check hour
   if( ( param_2 == NULL_PARAMETER ) || ( param_2 == RTC_1_GetHours(time) ))
   {
     if( ( param_3 == NULL_PARAMETER ) || ( param_3 == RTC_1_GetMinutes(time) ))
     {
       if( ( param_4 == NULL_PARAMETER ) || ( param_4 == RTC_1_GetSecond(time)))
       {
          return CF_DISABLE;
       }
     }
   }
    return CF_HALT;  // implemented on specific system
#endif
  return CF_DISABLE;
}


int wait_event_count_fn(CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned rx_event, 
    unsigned rx_event_count, unsigned param_4,  unsigned event, unsigned event_data)
{
  unsigned value; 
  int      return_value;
  
  if( event == CF_INIT_EVENT )
  {
       cf_store_cell_value( cf,link_id , 0 );
       return_value = CF_CONTINUE;

  }
  
   if( event == rx_event )
   {
       value = cf_get_cell_value( cf,link_id );
       value += 1;
       cf_store_cell_value(cf,link_id, value);
       if( value >= rx_event_count )
       {
          return_value = CF_DISABLE;
       }
       else
       {
	   return_value = CF_HALT;
       }
   }
   else
   {
      return_value = CF_HALT;
   }
   return return_value;   

}

  
int wait_event_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned rx_event, 
    unsigned time_count, unsigned param_4,  unsigned event, unsigned event_data)
{
   int       return_value;
   unsigned  value;
   if( event == CF_INIT_EVENT )
  {
       cf_store_cell_value( cf,link_id , 0 );
       return CF_CONTINUE;

  }
	
   if( event == rx_event )
   {
      return CF_DISABLE;
   }
  else if( event == CF_TIME_TICK_EVENT )
  {
    if( time_count > 0 )
    {
         value = cf_get_cell_value(cf, link_id );
         value += event_data;
         cf_store_cell_value(cf,link_id, value);

         if( value >= time_count )
         {
             return_value = CF_RESET;
          }
          else
          {
		      return_value = CF_HALT;
          }
	  }
      else
      {
        return CF_HALT;
      }
   }  
   else
   {
	
      return_value = CF_HALT;
   }
   return return_value;   

}



int wait_time_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
  int return_value;
  unsigned value;
  
  if( event == CF_INIT_EVENT )
  {
       cf_store_cell_value( cf,link_id , 0 );
       return CF_CONTINUE;

  }
  else if( event == CF_TIME_TICK_EVENT )
  {
     value = cf_get_cell_value(cf, link_id );
     value += event_data;
     cf_store_cell_value(cf,link_id, value);

     if( value >= param_2 )
     {
       return_value = CF_DISABLE;
     }
     else
     {
        return_value = CF_HALT;
     }
  }
  else
  {
       return_value = CF_HALT;
  }
  return return_value;

}

int wait_condition_fn(CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
    int return_value;
    unsigned value;
    if( event == CF_INIT_EVENT )
  {
        param_1( cf,link_id , param_2, param_3, param_4, event,event_data );
       cf_store_cell_value( cf,link_id , 0 );
       return CF_CONTINUE;

  }
   
   else if( param_1( cf,link_id , param_2, param_3, param_4, event,event_data ) != 0)
   {
     return_value = CF_DISABLE;
   }
   else if( ( event == CF_TIME_TICK_EVENT )&&(param_4 > 0 ) )
   {
     value = cf_get_cell_value(cf, link_id );
     value += event_data;
     cf_store_cell_value(cf,link_id, value);

     if( value >= param_4 )
     {
       return_value = CF_RESET;
     }
     else
     {
        return_value = CF_HALT;
     }
  }
   else
   {
     return_value = CF_HALT;
   }
 
   return return_value;

}     

int verify_condition_fn(CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   int return_value;
  
   
   if( param_1( cf,link_id , param_2, param_3, param_4, event,event_data ) != 0)
   {
     return_value = CF_CONTINUE;
   }
   else
   {
     return_value = CF_RESET;
   } 
   return return_value;

}    
 
int verify_not_timeout_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
  int return_value;
  unsigned value;
  
  if( event == CF_INIT_EVENT )
  {
       cf_store_cell_value( cf,link_id , 0 );
       return_value = CF_CONTINUE;

  }
  else if( event == CF_TIME_TICK_EVENT )
  {
     value = cf_get_cell_value( cf,link_id );
     value += event_data;
     cf_store_cell_value(cf,link_id, value);

     if( value >= param_2 )
     {
       return_value = CF_RESET;
     }
     else
     {
        return_value = CF_CONTINUE;
     }
  }
  else
  {
       return_value = CF_CONTINUE;
  }
  return return_value;

}


int verify_not_event_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
  
   int    return_value;
   
   if( event == param_2 )
   {
      return_value = CF_RESET;
   }
   else
   {
      return_value = CF_CONTINUE;
   }
   return return_value;   

}

int verify_not_tod_fn(CHAIN_FLOW_HANDLE *cf, unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   #if 0
   uint32 time;
   time = RTC_1_GetTime();
 
   // check hour
   if( ( param_2 == NULL_PARAMETER ) || ( param_2 == RTC_1_GetHours(time) ))
   {
     if( ( param_3 == NULL_PARAMETER ) || ( param_3 == RTC_1_GetMinutes(time) ))
     {
       if( ( param_4 == NULL_PARAMETER ) || ( param_4 == RTC_1_GetSecond(time) ))
       {
          return CF_RESET;
       }
     }
   }
   #endif
    return CF_CONTINUE;  // implemented on specific system

}

int nop_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{

    
   if( event != CF_INIT_EVENT )
   {
       return CF_DISABLE;
   }
   else
   {
       return CF_CONTINUE;
   }
 }
 

 
int change_state_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned chain_id, 
    unsigned link_index, unsigned param_4,  unsigned event, unsigned event_data)
{
#if 0
   if( event == CF_INIT_EVENT )
   {
      cf_change_state( cf,chain_id, link_index );
   }
#endif
   return CF_DISABLE;


}

int system_reset_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   abort();
   return CF_DISABLE;
}

int enable_chain_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   if( event == CF_INIT_EVENT )
   {
      if( param_2 != NULL_PARAMETER)
      {
          cf_enable_chain(cf,(intptr_t)param_2);
      }
      if( param_3 != NULL_PARAMETER)
      {
          cf_enable_chain(cf,(intptr_t)param_3);
      }

      if( param_4 != NULL_PARAMETER)
      {
          cf_enable_chain(cf,(intptr_t)param_4);
      }

   }
   return CF_DISABLE;

}
  
int disable_chain_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   if( event == CF_INIT_EVENT )
   {
     if( param_2 != NULL_PARAMETER)
      {
           cf_disable_chain(cf,(intptr_t)param_2);
      }
      if( param_3 != NULL_PARAMETER)
      {
          cf_suspend_chain(cf,(intptr_t)param_3);
      }

      if( param_4 != NULL_PARAMETER)
      {
          cf_suspend_chain(cf,(intptr_t)param_4);
      }

     
      
      
   }
   return CF_DISABLE;


}
int suspend_chain_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   if( event == CF_INIT_EVENT )
   {
 
      cf_suspend_chain(cf,(intptr_t)param_2);
      cf_suspend_chain(cf,(intptr_t)param_3);
      cf_suspend_chain(cf,(intptr_t)param_4);
   }
   return CF_DISABLE;
}

int resume_chain_fn( CHAIN_FLOW_HANDLE *cf,unsigned link_id, CF_FUN_AUX param_1, unsigned param_2, 
    unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data)
{
   if( event == CF_INIT_EVENT )
   {
      
      cf_resume_chain(cf,(intptr_t)param_2);
      cf_resume_chain(cf,(intptr_t)param_3);
      cf_resume_chain(cf,(intptr_t)param_4);
   }
   return CF_DISABLE;

}






