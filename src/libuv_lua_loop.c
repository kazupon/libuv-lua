/*
 * Event loop module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_loop.h"

/* other include(s) */
#include <assert.h>

/* define(s) */
#define LOOP_T  "libuv.loop"
#define DEFAULT_LOOP_T  "libuv.default_loop"

/* variable(s) */
static libuv_loop_t libuv_default_loop;


/* function(s) */

static int new_loop (lua_State *L) {
  libuv_loop_t *loop = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* create libuv_loop_t */
  loop = (libuv_loop_t *)lua_newuserdata(L, sizeof(libuv_loop_t)); /* [ userdata ] */
  if (loop == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create libuv_loop_t (%p)\n", loop);
  assert(loop != NULL);

  /* set metatable */
  luaL_getmetatable(L, LOOP_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* create event loop */
  loop->uvloop = NULL;
  loop->uvloop = uv_loop_new();
  /* TODO: should be implemeted by uv_last_error */
  if (loop->uvloop == NULL) {
    /* TODO: error */
    return 0;
  }
  loop->default_flag = 0;
  TRACE("new uv_loop_t (%p)\n", loop->uvloop);
  assert(loop->uvloop != NULL);
  assert(loop->default_flag == 0);

  return 1;
}

static int default_loop (lua_State *L) {
  libuv_loop_t *loop = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  lua_getfield(L, LUA_REGISTRYINDEX, DEFAULT_LOOP_T); /* [ userdata ] */
  loop = lua_touserdata(L, -1); /* [ userdata ] */
  if (loop == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("get default event loop (%p)\n", loop);
  assert(loop == &libuv_default_loop);
  assert(loop->default_flag == 1);

  return 1;
}

static int delete_loop (lua_State *L) {
  libuv_loop_t *loop = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  loop = (libuv_loop_t *)luaL_checkudata(L, 1, LOOP_T);
  TRACE("get libuv_loop_t (%p)\n", loop);
  assert(loop != NULL);

  /* release event loop (uv_loop_t) */
  if (!loop->default_flag) {
    if (loop->uvloop) {
      TRACE("deleting uv_loop_t (%p)\n", loop->uvloop);
      uv_loop_delete(loop->uvloop);
      /* TODO: should be implemeted by uv_last_error */
    }
    loop->uvloop = NULL;
  } else {
    TRACE("not release default event loop\n");
  }

  return 0;
}

static int run_loop (lua_State *L) {
  int ret = 0;
  libuv_loop_t *loop = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get first argument */
  loop = (libuv_loop_t *)luaL_checkudata(L, 1, LOOP_T); /* [ userdata ] */
  TRACE("get libuv_loop_t (%p)\n", loop);
  assert(loop != NULL);
  assert(loop->uvloop != NULL);

  /* execute uv_run */
  ret = uv_run(loop->uvloop);
  /* TODO: should be implemeted by uv_last_error */
  TRACE("uv_run: ret = %d\n", ret);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static int run_once_loop (lua_State *L) {
  int ret = 0;
  libuv_loop_t *loop = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get first argument */
  loop = (libuv_loop_t *)luaL_checkudata(L, 1, LOOP_T); /* [ userdata ] */
  TRACE("get libuv_loop_t (%p)\n", loop);
  assert(loop != NULL);
  assert(loop->uvloop != NULL);

  /* execute uv_run_once */
  ret = uv_run(loop->uvloop);
  /* TODO: should be implemeted by uv_last_error */
  TRACE("uv_run_once: ret = %d\n", ret);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static int update_time_loop (lua_State *L) {
  libuv_loop_t *loop;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get first argument */
  loop = NULL;
  loop = (libuv_loop_t *)luaL_checkudata(L, 1, LOOP_T); /* [ userdata ] */
  TRACE("get libuv_loop_t (%p)\n", loop);
  assert(loop != NULL);
  assert(loop->uvloop != NULL);

  /* execute uv_udate_time */
  uv_update_time(loop->uvloop);

  return 0;
}

static int now_loop (lua_State *L) {
  int ret = 0;
  libuv_loop_t *loop = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* get first argument */
  loop = (libuv_loop_t *)luaL_checkudata(L, 1, LOOP_T); /* [ userdata ] */
  TRACE("get libuv_loop_t (%p)\n", loop);
  assert(loop != NULL);
  assert(loop->uvloop != NULL);

  /* execute uv_now */
  ret = uv_now(loop->uvloop); // TODO: upcast?? 
  /* TODO: should be implemeted by uv_last_error */
  TRACE("uv_now: ret = %d\n", ret);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static const struct luaL_Reg libuv_loop_methods [] = {
  { "__gc", delete_loop },
  { "run", run_loop },
  { "run_once", run_once_loop },
  { "update_time", update_time_loop },
  { "now", now_loop },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_loop (lua_State *L) {

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_loop (lua_State *L) {
  int size = 0;
  uv_loop_t *def_loop = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* check stack size & type check ([ table ]) */
  size = lua_gettop(L);
  if (size != 1 || !lua_istable(L, 1)) {
    return 1;
  }
  assert(size == 1);

  /* create meta table */
  luaL_newmetatable(L, LOOP_T); /* [ table, metatable ] */
  TRACE("create libuv.loop metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.loop metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_loop_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist event loop methods\n");

  /* initialize default event loop */
  def_loop = NULL;
  def_loop = uv_default_loop();
  if (def_loop == NULL) {
    /* TODO: error */
    return 1;
  }
  libuv_default_loop.default_flag = 1;
  libuv_default_loop.uvloop = def_loop;
  lua_pushlightuserdata(L, &libuv_default_loop); /* [ table, userdata ] */
  luaL_getmetatable(L, LOOP_T); /* [ table, userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ table, userdata ] */
  TRACE("push light userdata (%p)\n", &libuv_default_loop);
  lua_setfield(L, LUA_REGISTRYINDEX, DEFAULT_LOOP_T); /* [ table ] */
  TRACE("regist default event loop (%p) to lua registry index\n", libuv_default_loop.uvloop);
  assert(lua_gettop(L) == 1);

  /* create Loop table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_loop); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_pushcfunction(L, default_loop); /* [ table, table, function ] */
  lua_setfield(L, -2, "default"); /* [ table, table ]*/
  TRACE("set 'default' function\n");

  lua_setfield(L, -2, "Loop"); /* [ table ] */
  TRACE("set 'Loop' attribute\n");

  return 0;
}
