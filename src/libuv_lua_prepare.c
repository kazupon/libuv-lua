/*
 * Prepare event event module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_prepare.h"

/* other include(s) */
#include <assert.h>
#include <stdlib.h>
#include "libuv_lua_loop.h"

/* define(s) */
#define PREPARE_T  "libuv.prepare"

/* variable(s) */


/* function(s) */

/* uv_close callback function */
static void on_close_uv_prepare_handle (uv_handle_t *handle) {
  lua_State *L = NULL;
  libuv_prepare_t *prepare = NULL;

  assert(handle != NULL);
  TRACE("arguments: handle = %p\n", handle);

  if (handle->data == NULL) {
    TRACE("not start prepare\n");
    return;
  }

  prepare = (libuv_prepare_t *)handle->data;
  L = prepare->L;

  luaL_unref(L, LUA_REGISTRYINDEX, prepare->ref);
  prepare->ref = LUA_NOREF;
  prepare->uvprepare->data = NULL;
  TRACE("free uv_prepare_t\n");
  free(prepare->uvprepare);
  prepare->uvprepare = NULL;
}

/* prepare callback function */
static void on_prepare (uv_prepare_t *handle, int status) {
  lua_State *L = NULL;
  libuv_prepare_t *prepare = NULL;

  assert(handle != NULL);
  TRACE("arguments: handle = %p, status = %d\n", handle, status);
  
  prepare = (libuv_prepare_t *)handle->data;
  assert(prepare != NULL);
  TRACE("libuv_prepare_t attributes: uvprepare = %p, L = %p, ref = 0x%012x\n", prepare->uvprepare, prepare->L, prepare->ref);

  L = prepare->L;
  assert(L != NULL);

  /* [ userdata ] */
  /* DUMP_STACK(L); */

  lua_rawgeti(L, LUA_REGISTRYINDEX, prepare->ref); /* [ userdata, userdata ] */
  assert(lua_gettop(L) == 2);

  lua_getfenv(L, -1); /* [ userdata, userdata, env_table ] */
  lua_getfield(L, -1, "on_prepare"); /* [ userdata, userdata, env_table, function ] */
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

static int new_prepare (lua_State *L) {
  int ret = 0;
  libuv_prepare_t *prepare = NULL;
  uv_prepare_t *uvprepare = NULL;
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

  /* create libuv_prepare_t */
  prepare = (libuv_prepare_t *)lua_newuserdata(L, sizeof(libuv_prepare_t)); /* [ userdata ] */
  if (prepare == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create libuv_prepare_t (%p)\n", prepare);
  assert(prepare != NULL);

  /* create uv_prepare_t */
  uvprepare = (uv_prepare_t *)malloc(sizeof(uv_prepare_t));
  if (uvprepare == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create uv_prepare_t (%p)\n", uvprepare);
  assert(uvprepare != NULL);

  /* libuv_prepare_t settings */
  prepare->uvprepare = uvprepare;
  prepare->L = L;
  prepare->ref = LUA_NOREF;

  /* set envriment table to userdata */
  lua_newtable(L); /* [ userdata, table ] */
  lua_setfenv(L, -2); /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* set metatable */
  luaL_getmetatable(L, PREPARE_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* initialize prepare */
  ret = uv_prepare_init(loop->uvloop, prepare->uvprepare);
  if (ret) {
    /* TODO: error */
    return 0;
  }
  TRACE("uv_prepare_init: ret = %d\n", ret);

  /* return userdata */
  return 1;
}

static int delete_prepare (lua_State *L) {
  libuv_prepare_t *prepare = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  prepare = (libuv_prepare_t *)luaL_checkudata(L, 1, PREPARE_T); /* [ userdata ] */
  assert(prepare != NULL);
  TRACE("libuv_prepare_t attributes: uvprepare = %p, L = %p, ref = 0x%012x\n", prepare->uvprepare, prepare->L, prepare->ref);

  if (prepare->uvprepare) {
    TRACE("release uv_prepare_t\n");
    uv_close((uv_handle_t *)prepare->uvprepare, on_close_uv_prepare_handle);
  } else {
    TRACE("already realeased uv_prepare_t\n");
    assert(prepare->uvprepare == NULL);
    assert(prepare->ref == LUA_NOREF);
  }

  return 0;
}

static int start_prepare (lua_State *L) {
  int ret = 0;
  int ref_ret = LUA_REFNIL;
  libuv_prepare_t *prepare = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata, function ] */
  assert(lua_gettop(L) == 2); 

  /* get callback argument */
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_getfenv(L, 1); /* [ userdata, function, env_table ] */
  lua_pushvalue(L, 2); /* [ userdata, function, env_table, function ] */
  lua_remove(L, 2); /* [ userdata, env_table, function ] */
  lua_setfield(L, -2, "on_prepare"); /* [ userdata, env_table ] */
  lua_pop(L, 1); /* [ userdata ] */

  /* get self argument */
  prepare = (libuv_prepare_t *)luaL_checkudata(L, 1, PREPARE_T);
  TRACE("get libuv_prepare_t (%p)\n", prepare);
  assert(prepare != NULL);
  assert(prepare->uvprepare != NULL);
  prepare->uvprepare->data = prepare;
  assert(prepare->uvprepare->data != NULL);

  /* execute uv_prepare_start */
  ret = uv_prepare_start(prepare->uvprepare, on_prepare);
  TRACE("uv_prepare_start: ret = %d\n", ret);

  /* set reference */
  TRACE("referencing uv_prepare_t\n");
  ref_ret = luaL_ref(L, LUA_REGISTRYINDEX);
  if (ref_ret == LUA_REFNIL) {
    /* TODO: error */
    fprintf(stderr, "FAIELD luaL_ref !!");
    return 0;
  }
  prepare->ref = ref_ret;

  TRACE("libuv_prepare_t attributes: uvprepare = %p, L = %p, ref = 0x%012x\n", prepare->uvprepare, prepare->L, prepare->ref);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static int stop_prepare (lua_State *L) {
  int ret = 0;
  libuv_prepare_t *prepare = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata ] */
  assert(lua_gettop(L) == 1); 

  /* get self argument */
  prepare = (libuv_prepare_t *)luaL_checkudata(L, 1, PREPARE_T);
  TRACE("get libuv_prepare_t (%p)\n", prepare);
  assert(prepare != NULL);
  assert(prepare->uvprepare != NULL);

  /* execute uv_prepare_stop */
  ret = uv_prepare_stop(prepare->uvprepare);
  TRACE("uv_prepare_stop: ret = %d\n", ret);

  /* set reference */
  TRACE("unreference uv_prepare_t\n");
  luaL_unref(L, LUA_REGISTRYINDEX, prepare->ref);
  prepare->ref = LUA_NOREF;

  TRACE("libuv_prepare_t attributes: uvprepare = %p, L = %p, ref = 0x%012x\n", prepare->uvprepare, prepare->L, prepare->ref);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static const struct luaL_Reg libuv_prepare_methods [] = {
  { "__gc", delete_prepare },
  { "close", delete_prepare },
  { "start", start_prepare },
  { "stop", stop_prepare },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_prepare (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_prepare (lua_State *L) {
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
  luaL_newmetatable(L, PREPARE_T); /* [ table, metatable ] */
  TRACE("create libuv.prepar emetatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.prepare metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_prepare_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist prepare methods\n");

  /* create prepare table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_prepare); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table ] */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "Prepare"); /* [ table ] */
  TRACE("set 'Prepare' attribute\n");

  return 0;
}
