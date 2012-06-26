/*
 * Readers writer lock module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_RWLOCK_H
#define LIBUV_LUA_RWLOCK_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


LUALIB_API int luaopen_libuv_rwlock (lua_State *L);
LUALIB_API int luaopenL_libuv_rwlock (lua_State *L);


#endif /* LIBUV_LUA_RWLOCK_H */
