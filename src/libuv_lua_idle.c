/*
 * Idle event module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_idle.h"

/* other include(s) */
#include <assert.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "libuv_lua_loop.h"

/* define(s) */
#define IDLE_T  "libuv.idle"

/* variable(s) */


/* function(s) */

/* uv_close callback function */
static void on_close_uv_idle_handle (uv_handle_t *handle) {
  lua_State *L = NULL;
  libuv_idle_t *idle = NULL;

  assert(handle != NULL);
  TRACE("arguments: handle = %p\n", handle);

  if (handle->data == NULL) {
    TRACE("not start idle\n");
    return;
  }

  idle = (libuv_idle_t *)handle->data;
  L = idle->L;

  luaL_unref(L, LUA_REGISTRYINDEX, idle->ref);
  idle->ref = LUA_NOREF;
  idle->uvidle->data = NULL;
  TRACE("free uv_idle_t\n");
  free(idle->uvidle);
  idle->uvidle = NULL;
}

/* idle callback function */
static void on_idle (uv_idle_t *handle, int status) {
  lua_State *L = NULL;
  libuv_idle_t *idle = NULL;

  assert(handle != NULL);
  TRACE("arguments: handle = %p, status = %d\n", handle, status);
  
  idle = (libuv_idle_t *)handle->data;
  assert(idle != NULL);
  TRACE("libuv_idle_t attributes: uvidle = %p, L = %p, ref = 0x%012x\n", idle->uvidle, idle->L, idle->ref);

  L = idle->L;
  assert(L != NULL);

  /* [ userdata ] */
  /* DUMP_STACK(L); */

  lua_rawgeti(L, LUA_REGISTRYINDEX, idle->ref); /* [ userdata, userdata ] */
  assert(lua_gettop(L) == 2);

  lua_getfenv(L, -1); /* [ userdata, userdata, env_table ] */
  lua_getfield(L, -1, "on_idle"); /* [ userdata, userdata, env_table, function ] */
  if (!lua_isfunction(L, -1)) { /* check handler */
    /* TODO: error */
    fprintf(stderr, "Invalid handler");
    return;
  }

  lua_pushvalue(L, 2); /* [ userdata, userdata, env_table, function, userdata ] */
  lua_pushinteger(L, status); /* [ userdata, userdata, env_table, function, userdata, number ] */
  lua_call(L, 2, 0);
  lua_settop(L, 1);
}

static int new_idle (lua_State *L) {
  int ret = 0;
  libuv_idle_t *idle = NULL;
  uv_idle_t *uvidle = NULL;
  libuv_loop_t *loop = NULL;

  /* [ userdata ] */
  /* DUMP_STACK(L); */

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get first argument */
  loop = (libuv_loop_t *)lua_touserdata(L, 1); /* [userdata ] */
  if (loop == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("get libuv_loop_t (%p)\n", loop);
  assert(loop->uvloop != NULL);
  lua_pop(L, 1); /* [ ] */
  assert(lua_gettop(L) == 0);

  /* create libuv_idle_t */
  idle = (libuv_idle_t *)lua_newuserdata(L, sizeof(libuv_idle_t)); /* [ userdata ] */
  if (idle == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create libuv_idle_t (%p)\n", idle);
  assert(idle != NULL);

  /* create uv_idle_t */
  uvidle = (uv_idle_t *)malloc(sizeof(uv_idle_t));
  if (uvidle == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create uv_idle_t (%p)\n", uvidle);
  assert(uvidle != NULL);

  /* libuv_idle_t settings */
  idle->uvidle = uvidle;
  idle->L = L;
  idle->ref = LUA_NOREF;

  /* set envriment table to userdata */
  lua_newtable(L); /* [ userdata, table ] */
  lua_setfenv(L, -2); /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* set metatable */
  luaL_getmetatable(L, IDLE_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* initialize idle */
  ret = uv_idle_init(loop->uvloop, idle->uvidle);
  if (ret) {
    /* TODO: error */
    return 0;
  }
  TRACE("uv_idle_init: ret = %d\n", ret);

  /* return userdata */
  return 1;
}

static int delete_idle (lua_State *L) {
  libuv_idle_t *idle = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  idle = (libuv_idle_t *)luaL_checkudata(L, 1, IDLE_T); /* [ userdata ] */
  assert(idle != NULL);
  TRACE("libuv_idle_t attributes: uvidle = %p, L = %p, ref = 0x%012x\n", idle->uvidle, idle->L, idle->ref);

  if (idle->uvidle) {
    TRACE("release uv_idle_t\n");
    uv_close((uv_handle_t *)idle->uvidle, on_close_uv_idle_handle);
  } else {
    TRACE("already realeased uv_idle_t\n");
    assert(idle->uvidle == NULL);
    assert(idle->ref == LUA_NOREF);
  }

  return 0;
}

static int start_idle (lua_State *L) {
  int ret = 0;
  int ref_ret = LUA_REFNIL;
  libuv_idle_t *idle = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata, function ] */
  assert(lua_gettop(L) == 2); 

  /* get callback argument */
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_getfenv(L, 1); /* [ userdata, function, env_table ] */
  lua_pushvalue(L, 2); /* [ userdata, function, env_table, function ] */
  lua_remove(L, 2); /* [ userdata, env_table, function ] */
  lua_setfield(L, -2, "on_idle"); /* [ userdata, env_table ] */
  lua_pop(L, 1); /* [ userdata ] */

  /* get self argument */
  idle = (libuv_idle_t *)luaL_checkudata(L, 1, IDLE_T);
  TRACE("get libuv_idle_t (%p)\n", idle);
  assert(idle != NULL);
  assert(idle->uvidle != NULL);
  idle->uvidle->data = idle;
  assert(idle->uvidle->data != NULL);

  /* execute uv_idle_start */
  ret = uv_idle_start(idle->uvidle, on_idle);
  TRACE("uv_idle_start: ret = %d\n", ret);

  /* set reference */
  TRACE("referencing uv_idle_t\n");
  ref_ret = luaL_ref(L, LUA_REGISTRYINDEX);
  if (ref_ret == LUA_REFNIL) {
    /* TODO: error */
    fprintf(stderr, "FAIELD luaL_ref !!");
    return 0;
  }
  idle->ref = ref_ret;

  TRACE("libuv_idle_t attributes: uvidle = %p, L = %p, ref = 0x%012x\n", idle->uvidle, idle->L, idle->ref);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static int stop_idle (lua_State *L) {
  int ret = 0;
  libuv_idle_t *idle = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata ] */
  assert(lua_gettop(L) == 1); 

  /* get self argument */
  idle = (libuv_idle_t *)luaL_checkudata(L, 1, IDLE_T);
  TRACE("get libuv_idle_t (%p)\n", idle);
  assert(idle != NULL);
  assert(idle->uvidle != NULL);

  /* execute uv_idle_stop */
  ret = uv_idle_stop(idle->uvidle);
  TRACE("uv_idle_stop: ret = %d\n", ret);

  /* set reference */
  TRACE("unreference uv_idle_t\n");
  luaL_unref(L, LUA_REGISTRYINDEX, idle->ref);
  idle->ref = LUA_NOREF;

  TRACE("libuv_idle_t attributes: uvidle = %p, L = %p, ref = 0x%012x\n", idle->uvidle, idle->L, idle->ref);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static const struct luaL_Reg libuv_idle_methods [] = {
  { "__gc", delete_idle },
  { "close", delete_idle },
  { "start", start_idle },
  { "stop", stop_idle },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_idle (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_idle (lua_State *L) {
  int size = 0;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* check stack size & type check ([ table ]) */
  size = lua_gettop(L);
  if (size != 1 || !lua_istable(L, 1)) {
    return 1;
  }
  assert(size == 1);

  /* create meta table */
  luaL_newmetatable(L, IDLE_T); /* [ table, metatable ] */
  TRACE("create libuv.idle metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.idle metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_idle_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist idle methods\n");

  /* create Idle table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_idle); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "Idle"); /* [ table ] */
  TRACE("set 'Idle' attribute\n");

  return 0;
}
