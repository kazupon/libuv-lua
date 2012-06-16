/*
 * Timer module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_TIMER_H
#define LIBUV_LUA_TIMER_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


typedef struct libuv_timer_t {
  uv_timer_t *uvtimer;
  lua_State *L;
  int ref;
  int ref_count;
} libuv_timer_t;


LUALIB_API int luaopen_libuv_timer (lua_State *L);
LUALIB_API int luaopenL_libuv_timer (lua_State *L);


#endif /* LIBUV_LUA_TIMER_H */
