/*
 * Prepare event module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_PREPARE_H
#define LIBUV_LUA_PREPARE_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


typedef struct libuv_prepare_t {
  uv_prepare_t *uvprepare;
  lua_State *L;
  int ref;
} libuv_prepare_t;


LUALIB_API int luaopen_libuv_prepare (lua_State *L);
LUALIB_API int luaopenL_libuv_prepare (lua_State *L);


#endif /* LIBUV_LUA_PREPARE_H */
