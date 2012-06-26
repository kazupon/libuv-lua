/*
 * Readers writer lock module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_rwlock.h"

/* other include(s) */
#include <assert.h>

/* define(s) */
#define RWLOCK_T "libuv.rwlock"

/* variable(s) */


/* function(s) */

static int new_rwlock (lua_State *L) {
  int ret = 0;
  uv_rwlock_t *rwlock = NULL;

  /* [ ] */
  /* DUMP_STACK(L); */
  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);
  assert(lua_gettop(L) == 0);

  /* create uv_rwlock_t */
  rwlock = (uv_rwlock_t *)lua_newuserdata(L, sizeof(uv_rwlock_t)); /* [ userdata ] */
  if (rwlock == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create uv_rwlock_t (%p)\n", rwlock);
  assert(rwlock != NULL);

  /* set envriment table to userdata */
  lua_newtable(L); /* [ userdata, table ] */
  lua_setfenv(L, -2); /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* set metatable */
  luaL_getmetatable(L, RWLOCK_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* initialize rwlock */
  ret = uv_rwlock_init(rwlock);
  if (ret) {
    /* TODO: error */
    return 0;
  }
  TRACE("uv_rwlock_init: ret = %d\n", ret);

  /* return userdata */
  return 1;
}

static int delete_rwlock (lua_State *L) {
  uv_rwlock_t *rwlock = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  rwlock = (uv_rwlock_t *)luaL_checkudata(L, 1, RWLOCK_T); /* [ userdata ] */
  TRACE("uv_rwlock_t = %p\n", rwlock);
  assert(rwlock != NULL);

  uv_rwlock_destroy(rwlock);
  TRACE("execute uv_rwlock_destroy\n");

  return 0;
}

static int rdlock_rwlock (lua_State *L) {
  uv_rwlock_t *rwlock = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get self argument */
  rwlock = (uv_rwlock_t *)luaL_checkudata(L, 1, RWLOCK_T); /* [ userdata ] */
  TRACE("uv_rwlock_t = %p\n", rwlock);
  assert(rwlock != NULL);

  uv_rwlock_rdlock(rwlock);
  TRACE("execute uv_rwlock_rdlock\n");

  return 0;
}

static int rdunlock_rwlock (lua_State *L) {
  uv_rwlock_t *rwlock = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get self argument */
  rwlock = (uv_rwlock_t *)luaL_checkudata(L, 1, RWLOCK_T); /* [ userdata ] */
  TRACE("uv_rwlock_t = %p\n", rwlock);
  assert(rwlock != NULL);

  uv_rwlock_rdunlock(rwlock);
  TRACE("execute uv_rwlock_rdunlock\n");

  return 0;
}

static int wrlock_rwlock (lua_State *L) {
  uv_rwlock_t *rwlock = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get self argument */
  rwlock = (uv_rwlock_t *)luaL_checkudata(L, 1, RWLOCK_T); /* [ userdata ] */
  TRACE("uv_rwlock_t = %p\n", rwlock);
  assert(rwlock != NULL);

  uv_rwlock_wrlock(rwlock);
  TRACE("execute uv_rwlock_wrlock\n");

  return 0;
}

static int wrunlock_rwlock (lua_State *L) {
  uv_rwlock_t *rwlock = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get self argument */
  rwlock = (uv_rwlock_t *)luaL_checkudata(L, 1, RWLOCK_T); /* [ userdata ] */
  TRACE("uv_rwlock_t = %p\n", rwlock);
  assert(rwlock != NULL);

  uv_rwlock_wrunlock(rwlock);
  TRACE("execute uv_rwlock_wrunlock\n");

  return 0;
}

static const struct luaL_Reg libuv_rwlock_methods [] = {
  { "__gc", delete_rwlock },
  { "rdlock", rdlock_rwlock },
  { "rdunlock", rdunlock_rwlock },
  { "wrlock", wrlock_rwlock },
  { "wrunlock", wrunlock_rwlock },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_rwlock (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_rwlock (lua_State *L) {
  int size = 0;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* rwlock stack size & type rwlock ([ table ]) */
  size = lua_gettop(L);
  if (size != 1 || !lua_istable(L, 1)) {
    return 1;
  }
  assert(size == 1);

  /* create meta table */
  luaL_newmetatable(L, RWLOCK_T); /* [ table, metatable ] */
  TRACE("create libuv.rwlock metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.rwlock metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_rwlock_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist rwlock methods\n");

  /* create rwlock table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_rwlock); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "RWLock"); /* [ table ] */
  TRACE("set 'RWLock' attribute\n");

  return 0;
}

