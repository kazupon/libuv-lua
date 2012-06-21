/*
 * Utilities
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_UTILS_H
#define LIBUV_LUA_UTILS_H


#include <stdlib.h>
#include "lua.h"


void set_field_integer (lua_State *L, const char *name, int32_t value);


#endif /* LIBUV_LUA_UTILS_H */
