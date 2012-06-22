/*
 * Idle event module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_IDLE_H
#define LIBUV_LUA_IDLE_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


typedef struct libuv_idle_t {
  uv_idle_t *uvidle;
  lua_State *L;
  int ref;
} libuv_idle_t;


LUALIB_API int luaopen_libuv_idle (lua_State *L);
LUALIB_API int luaopenL_libuv_idle (lua_State *L);


#endif /* LIBUV_LUA_IDLE_H */
