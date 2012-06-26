/*
 * Mutex module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_mutex.h"

/* other include(s) */
#include <assert.h>

/* define(s) */
#define MUTEX_T "libuv.mutex"

/* variable(s) */


/* function(s) */

static int new_mutex (lua_State *L) {
  int ret = 0;
  uv_mutex_t *mutex = NULL;

  /* [ ] */
  /* DUMP_STACK(L); */
  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);
  assert(lua_gettop(L) == 0);

  /* create uv_mutex_t */
  mutex = (uv_mutex_t *)lua_newuserdata(L, sizeof(uv_mutex_t)); /* [ userdata ] */
  if (mutex == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create uv_mutex_t (%p)\n", mutex);
  assert(mutex != NULL);

  /* set envriment table to userdata */
  lua_newtable(L); /* [ userdata, table ] */
  lua_setfenv(L, -2); /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* set metatable */
  luaL_getmetatable(L, MUTEX_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* initialize mutex */
  ret = uv_mutex_init(mutex);
  if (ret) {
    /* TODO: error */
    return 0;
  }
  TRACE("uv_mutex_init: ret = %d\n", ret);

  /* return userdata */
  return 1;
}

static int delete_mutex (lua_State *L) {
  uv_mutex_t *mutex = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  mutex = (uv_mutex_t *)luaL_checkudata(L, 1, MUTEX_T); /* [ userdata ] */
  TRACE("uv_mutex_t = %p\n", mutex);
  assert(mutex != NULL);

  uv_mutex_destroy(mutex);
  TRACE("execute uv_mutex_destroy\n");

  return 0;
}

static int lock_mutex (lua_State *L) {
  uv_mutex_t *mutex = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get self argument */
  mutex = (uv_mutex_t *)luaL_checkudata(L, 1, MUTEX_T); /* [ userdata ] */
  TRACE("uv_mutex_t = %p\n", mutex);
  assert(mutex != NULL);

  uv_mutex_lock(mutex);
  TRACE("execute uv_mutex_lock\n");

  return 0;
}

static int unlock_mutex (lua_State *L) {
  uv_mutex_t *mutex = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get self argument */
  mutex = (uv_mutex_t *)luaL_checkudata(L, 1, MUTEX_T); /* [ userdata ] */
  TRACE("uv_mutex_t = %p\n", mutex);
  assert(mutex != NULL);

  uv_mutex_unlock(mutex);
  TRACE("execute uv_mutex_unlock\n");

  return 0;
}

static const struct luaL_Reg libuv_mutex_methods [] = {
  { "__gc", delete_mutex },
  { "lock", lock_mutex },
  { "unlock", unlock_mutex },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_mutex (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_mutex (lua_State *L) {
  int size = 0;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* mutex stack size & type mutex ([ table ]) */
  size = lua_gettop(L);
  if (size != 1 || !lua_istable(L, 1)) {
    return 1;
  }
  assert(size == 1);

  /* create meta table */
  luaL_newmetatable(L, MUTEX_T); /* [ table, metatable ] */
  TRACE("create libuv.mutex metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.mutex metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_mutex_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist mutex methods\n");

  /* create mutex table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_mutex); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "Mutex"); /* [ table ] */
  TRACE("set 'Mutex' attribute\n");

  return 0;
}

