/*
 * Utilites
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_utils.h"


void set_field_integer (lua_State *L, const char *name, int32_t value) {
  lua_pushinteger(L, value);
  lua_setfield(L, -2, name);
}
