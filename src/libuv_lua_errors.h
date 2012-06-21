/*
 * Error module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_ERRORS_H
#define LIBUV_LUA_ERRORS_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"
#include "libuv_lua_utils.h"


LUALIB_API int luaopen_libuv_errors (lua_State *L);
LUALIB_API int luaopenL_libuv_errors (lua_State *L);


#endif /* LIBUV_LUA_ERRORS_H */
