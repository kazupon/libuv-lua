/*
 * Check event module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_check.h"

/* other include(s) */
#include <assert.h>
#include <stdlib.h>
#include "libuv_lua_loop.h"

/* define(s) */
#define CHECK_T  "libuv.check"

/* variable(s) */


/* function(s) */

/* uv_close callback function */
static void on_close_uv_check_handle (uv_handle_t *handle) {
  lua_State *L = NULL;
  libuv_check_t *check = NULL;

  assert(handle != NULL);
  TRACE("arguments: handle = %p\n", handle);

  if (handle->data == NULL) {
    TRACE("not start check\n");
    return;
  }

  check = (libuv_check_t *)handle->data;
  L = check->L;

  luaL_unref(L, LUA_REGISTRYINDEX, check->ref);
  check->ref = LUA_NOREF;
  check->uvcheck->data = NULL;
  TRACE("free uv_check_t\n");
  free(check->uvcheck);
  check->uvcheck = NULL;
}

/* check callback function */
static void on_check (uv_check_t *handle, int status) {
  lua_State *L = NULL;
  libuv_check_t *check = NULL;

  assert(handle != NULL);
  TRACE("arguments: handle = %p, status = %d\n", handle, status);
  
  check = (libuv_check_t *)handle->data;
  assert(check != NULL);
  TRACE("libuv_check_t attributes: uvcheck = %p, L = %p, ref = 0x%012x\n", check->uvcheck, check->L, check->ref);

  L = check->L;
  assert(L != NULL);

  /* [ userdata ] */
  /* DUMP_STACK(L); */

  lua_rawgeti(L, LUA_REGISTRYINDEX, check->ref); /* [ userdata, userdata ] */
  assert(lua_gettop(L) == 2);

  lua_getfenv(L, -1); /* [ userdata, userdata, env_table ] */
  lua_getfield(L, -1, "on_check"); /* [ userdata, userdata, env_table, function ] */
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

static int new_check (lua_State *L) {
  int ret = 0;
  libuv_check_t *check = NULL;
  uv_check_t *uvcheck = NULL;
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

  /* create libuv_check_t */
  check = (libuv_check_t *)lua_newuserdata(L, sizeof(libuv_check_t)); /* [ userdata ] */
  if (check == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create libuv_check_t (%p)\n", check);
  assert(check != NULL);

  /* create uv_check_t */
  uvcheck = (uv_check_t *)malloc(sizeof(uv_check_t));
  if (uvcheck == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create uv_check_t (%p)\n", uvcheck);
  assert(uvcheck != NULL);

  /* libuv_check_t settings */
  check->uvcheck = uvcheck;
  check->L = L;
  check->ref = LUA_NOREF;

  /* set envriment table to userdata */
  lua_newtable(L); /* [ userdata, table ] */
  lua_setfenv(L, -2); /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* set metatable */
  luaL_getmetatable(L, CHECK_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* initialize check */
  ret = uv_check_init(loop->uvloop, check->uvcheck);
  if (ret) {
    /* TODO: error */
    return 0;
  }
  TRACE("uv_check_init: ret = %d\n", ret);

  /* return userdata */
  return 1;
}

static int delete_check (lua_State *L) {
  libuv_check_t *check = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  check = (libuv_check_t *)luaL_checkudata(L, 1, CHECK_T); /* [ userdata ] */
  assert(check != NULL);
  TRACE("libuv_check_t attributes: uvcheck = %p, L = %p, ref = 0x%012x\n", check->uvcheck, check->L, check->ref);

  if (check->uvcheck) {
    TRACE("release uv_check_t\n");
    uv_close((uv_handle_t *)check->uvcheck, on_close_uv_check_handle);
  } else {
    TRACE("already realeased uv_check_t\n");
    assert(check->uvcheck == NULL);
    assert(check->ref == LUA_NOREF);
  }

  return 0;
}

static int start_check (lua_State *L) {
  int ret = 0;
  int ref_ret = LUA_REFNIL;
  libuv_check_t *check = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata, function ] */
  assert(lua_gettop(L) == 2); 

  /* get callback argument */
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_getfenv(L, 1); /* [ userdata, function, env_table ] */
  lua_pushvalue(L, 2); /* [ userdata, function, env_table, function ] */
  lua_remove(L, 2); /* [ userdata, env_table, function ] */
  lua_setfield(L, -2, "on_check"); /* [ userdata, env_table ] */
  lua_pop(L, 1); /* [ userdata ] */

  /* get self argument */
  check = (libuv_check_t *)luaL_checkudata(L, 1, CHECK_T);
  TRACE("get libuv_check_t (%p)\n", check);
  assert(check != NULL);
  assert(check->uvcheck != NULL);
  check->uvcheck->data = check;
  assert(check->uvcheck->data != NULL);

  /* execute uv_check_start */
  ret = uv_check_start(check->uvcheck, on_check);
  TRACE("uv_check_start: ret = %d\n", ret);

  /* set reference */
  TRACE("referencing uv_check_t\n");
  ref_ret = luaL_ref(L, LUA_REGISTRYINDEX);
  if (ref_ret == LUA_REFNIL) {
    /* TODO: error */
    fprintf(stderr, "FAIELD luaL_ref !!");
    return 0;
  }
  check->ref = ref_ret;

  TRACE("libuv_check_t attributes: uvcheck = %p, L = %p, ref = 0x%012x\n", check->uvcheck, check->L, check->ref);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static int stop_check (lua_State *L) {
  int ret = 0;
  libuv_check_t *check = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata ] */
  assert(lua_gettop(L) == 1); 

  /* get self argument */
  check = (libuv_check_t *)luaL_checkudata(L, 1, CHECK_T);
  TRACE("get libuv_check_t (%p)\n", check);
  assert(check != NULL);
  assert(check->uvcheck != NULL);

  /* execute uv_check_stop */
  ret = uv_check_stop(check->uvcheck);
  TRACE("uv_check_stop: ret = %d\n", ret);

  /* set reference */
  TRACE("unreference uv_check_t\n");
  luaL_unref(L, LUA_REGISTRYINDEX, check->ref);
  check->ref = LUA_NOREF;

  TRACE("libuv_check_t attributes: uvcheck = %p, L = %p, ref = 0x%012x\n", check->uvcheck, check->L, check->ref);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static const struct luaL_Reg libuv_check_methods [] = {
  { "__gc", delete_check },
  { "close", delete_check },
  { "start", start_check },
  { "stop", stop_check },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_check (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_check (lua_State *L) {
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
  luaL_newmetatable(L, CHECK_T); /* [ table, metatable ] */
  TRACE("create libuv.check metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.check metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_check_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist check methods\n");

  /* create Check table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_check); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "Check"); /* [ table ] */
  TRACE("set 'Check' attribute\n");

  return 0;
}

