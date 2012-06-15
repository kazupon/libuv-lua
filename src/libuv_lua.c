/*
 * main module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */


#include "libuv_lua.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "lua.h"
#include "lauxlib.h"

#include "libuv_lua_debug.h"
#include "libuv_lua_loop.h"


void debug_printf (const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}


int luaopen_libuvlua (lua_State *L) {
  /* [ string ] */

  int ret;
  assert(L != NULL);

  /* create a table for namespace */
  lua_settop(L, 0); /* [ ] */
  lua_newtable(L); /* [ table ] */

  /* load event loop module */
  ret = 0;
  ret = luaopenL_libuv_loop(L); /* [ table ] */
  if (ret) {
    /* TODO: should be error */
    lua_pushstring(L, "load libvu.loop error");
    lua_error(L);
    return 0;
  }

  return 1;
}

