/*
 * Task module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_TASK_H
#define LIBUV_LUA_TASK_H

#include "lua.h"
#include "lauxlib.h"
#include "uv.h"

#include "libuv_lua_debug.h"


typedef struct libuv_task_t {
  uv_work_t *uvtask;
  uv_loop_t *loop;
  lua_State *L;
  void *data;
  int ref;
} libuv_task_t;


LUALIB_API int luaopen_libuv_task (lua_State *L);
LUALIB_API int luaopenL_libuv_task (lua_State *L);


#endif /* LIBUV_LUA_TASK_H */
