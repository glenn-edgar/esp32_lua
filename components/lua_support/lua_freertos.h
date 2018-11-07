#ifndef _LUA_FREERTOS_H_
#define _LUA_FREERTOS_H_


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void free_rtos_create_queue(void);

LUALIB_API int luaopen_freertos(lua_State *L); 

#endif