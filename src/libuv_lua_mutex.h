/*
 * Mutex module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_MUTEX_H
#define LIBUV_LUA_MUTEX_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


LUALIB_API int luaopen_libuv_mutex (lua_State *L);
LUALIB_API int luaopenL_libuv_mutex (lua_State *L);


#endif /* LIBUV_LUA_MUTEX_H */
