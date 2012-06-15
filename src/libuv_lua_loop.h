/*
 * Event loop module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_LOOP_H
#define LIBUV_LUA_LOOP_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


typedef struct libuv_loop_t {
  lua_State *L; /* TODO: remove? */
  int default_flag;
  uv_loop_t *uvloop;
} libuv_loop_t;


LUALIB_API int luaopen_libuv_loop (lua_State *L);
LUALIB_API int luaopenL_libuv_loop (lua_State *L);


#endif /* LIBUV_LUA_LOOP_H */
