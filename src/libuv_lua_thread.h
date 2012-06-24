/*
 * Thread module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_THREAD_H
#define LIBUV_LUA_THREAD_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


typedef struct libuv_thread_t {
  uv_thread_t uvthread;
  lua_State *L;
} libuv_thread_t;

LUALIB_API int luaopen_libuv_thread (lua_State *L);
LUALIB_API int luaopenL_libuv_thread (lua_State *L);


#endif /* LIBUV_LUA_THREAD_H */
