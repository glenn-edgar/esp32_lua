


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
#include "watchdog.h"


static int wd_register (lua_State *L) 
{
    wdt_task_subscribe();   
    return 0;
}

static int wd_feed(lua_State *L) 
{
    wdt_reset_task_time();
    return 0;
}

/* -------------------------------------------------------------------------- */
static const struct luaL_Reg cmds[] = 
{
    
   
    {"register", wd_register },
    {"feed",wd_feed},
    {0}
};


static  int  luaopen_watchdog_package (lua_State *L) {
  luaL_newlib(L, cmds);
  return 1;
}


LUALIB_API int luaopen_watchdog(lua_State *L) 
{
    luaL_requiref(L, "freeRTOS", luaopen_watchdog_package, 1);
    lua_pop(L, 1);  /* remove lib */ 
   
  return 1;   
    
    
    
}