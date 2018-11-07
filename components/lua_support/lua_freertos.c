#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define MAX_QUEUE 10
#define QUEUE_LENGTH 10
QueueHandle_t queue_array[MAX_QUEUE];

void free_rtos_create_queue(void) 
{ 

   for(int i = 0 ; i < MAX_QUEUE; i++)
   {
     queue_array[i] =    xQueueCreate( QUEUE_LENGTH,sizeof( uint32_t *) );
                            
       
   }
   
   
}

static int free_rtos_delay(lua_State *L) 
{ 
  uint32_t delay_ms;
  delay_ms =  luaL_checkinteger(L, 1);
  vTaskDelay(delay_ms/ portTICK_PERIOD_MS);
  return 0;
}
static int free_rtos_send_msg(lua_State *L) 
{
   uint32_t queue_number;
   const char *input_data;
   char *queue_data;
   bool result;
   uint32_t len;
   char    *dest_ptr;
   
   queue_number =  luaL_checkinteger (L, 1);
   if(queue_number >= MAX_QUEUE)
   {
       abort();
       
   }
   input_data = lua_tolstring (L, 2,&len);  
   queue_data = malloc(len+4);
   memcpy(queue_data,(char*)&len,sizeof(uint32_t));
   dest_ptr = queue_data + (uint32_t)sizeof(uint32_t);
   memcpy(dest_ptr,input_data,len);
   result = xQueueSend(queue_array[queue_number],(void *)queue_data,0);
   lua_pushboolean(L,result);

   return 1;
}
static int free_rtos_pend_msg(lua_State *L) 
{
   uint32_t queue_number;
   uint32_t  max_delay;
   uint32_t  *len;
   char *input_data;
   char **queue_data;
   bool result;
   
   queue_number =  luaL_checkinteger (L, 1);
   if(queue_number >= MAX_QUEUE)
   {
       abort();
       
   }
   max_delay =  luaL_checkinteger (L, 1);

   result =  xQueueReceive(queue_array[queue_number],(void *)&queue_data,max_delay);
   lua_pushboolean(L,result);
   if( result > 0)
   {
      if( *queue_data != NULL )
      {
         len = (uint32_t *)queue_data;
         input_data = *queue_data+sizeof(uint32_t);        
         lua_pushlstring (L, input_data, *len);
         free(queue_data);
      }
      else
      {
         lua_pushnil(L);
      }
   }
   else
   {
       lua_pushnil(L);
       
   }

   return 2;
    
}



/* -------------------------------------------------------------------------- */
static const struct luaL_Reg cmds[] = 
{
    
   
    {"delay", free_rtos_delay },
    {"send_msg",free_rtos_send_msg},
    {"pend_msg",free_rtos_pend_msg},
    
    {0}
};


static  int  luaopen_freertos_package (lua_State *L) {
  luaL_newlib(L, cmds);
  return 1;
}


LUALIB_API int luaopen_freertos(lua_State *L) 
{
    luaL_requiref(L, "freeRTOS", luaopen_freertos_package, 1);
    lua_pop(L, 1);  /* remove lib */ 
   
  return 1;   
    
    
    
}