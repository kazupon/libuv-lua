/*
 * Check event module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_CHECK_H
#define LIBUV_LUA_CHECK_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


typedef struct libuv_check_t {
  uv_check_t *uvcheck;
  lua_State *L;
  int ref;
} libuv_check_t;


LUALIB_API int luaopen_libuv_check (lua_State *L);
LUALIB_API int luaopenL_libuv_check (lua_State *L);


#endif /* LIBUV_LUA_CHECK_H */
