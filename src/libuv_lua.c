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
#include "libuv_lua_errors.h"
#include "libuv_lua_timer.h"
#include "libuv_lua_idle.h"
#include "libuv_lua_check.h"
#include "libuv_lua_prepare.h"


void debug_printf (const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

int luaopen_libuvlua (lua_State *L) {
  /* [ string ] */

  int ret = 0;
  assert(L != NULL);

  /* create a table for namespace */
  lua_settop(L, 0); /* [ ] */
  lua_newtable(L); /* [ table ] */

  /* load errors module */
  ret = luaopenL_libuv_errors(L);
  if (ret) {
    /* TODO: should be error */
    lua_pushstring(L, "load libvu.Errors error");
    lua_error(L);
    return 0;
  }

  /* load event loop module */
  ret = 0;
  ret = luaopenL_libuv_loop(L); /* [ table ] */
  if (ret) {
    /* TODO: should be error */
    lua_pushstring(L, "load libvu.loop error");
    lua_error(L);
    return 0;
  }

  /* load timer moudle */
  ret = 0;
  ret = luaopenL_libuv_timer(L); /* [ table ] */
  if (ret) {
    /* TODO: should be error */
    lua_pushstring(L, "load libvu.timer error");
    lua_error(L);
    return 0;
  }

  /* load idle event moudle */
  ret = 0;
  ret = luaopenL_libuv_idle(L); /* [ table ] */
  if (ret) {
    /* TODO: should be error */
    lua_pushstring(L, "load libvu.idle error");
    lua_error(L);
    return 0;
  }

  /* load check event moudle */
  ret = 0;
  ret = luaopenL_libuv_check(L); /* [ table ] */
  if (ret) {
    /* TODO: should be error */
    lua_pushstring(L, "load libvu.check error");
    lua_error(L);
    return 0;
  }

  /* load prepare event moudle */
  ret = 0;
  ret = luaopenL_libuv_prepare(L); /* [ table ] */
  if (ret) {
    /* TODO: should be error */
    lua_pushstring(L, "load libvu.prepare error");
    lua_error(L);
    return 0;
  }

  return 1;
}

