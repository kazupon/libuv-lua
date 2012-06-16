/*
 * Timer module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_timer.h"

/* other include(s) */
#include <assert.h>
#include <stdlib.h>
#include "libuv_lua_loop.h"

/* define(s) */
#define TIMER_T  "libuv.timer"

/* variable(s) */


/* function(s) */

/* uv_close callback function */
static void on_close_uv_timer_handle (uv_handle_t *handle) {
  lua_State *L = NULL;
  libuv_timer_t *timer = NULL;

  assert(handle != NULL);
  TRACE("arguments: handle = %p\n", handle);

  timer = (libuv_timer_t *)handle->data;
  L = timer->L;
  luaL_unref(L, LUA_REGISTRYINDEX, timer->ref);
  timer->ref_count = 0;
  timer->ref = LUA_NOREF;
  timer->uvtimer->data = NULL;
  TRACE("free uv_timer_t\n");
  free(timer->uvtimer);
  timer->uvtimer = NULL;
}

/* timer callback function */
static void on_timer (uv_timer_t *handle, int status) {
  lua_State *L = NULL;
  libuv_timer_t *timer = NULL;

  assert(handle != NULL);
  assert(!uv_is_active((uv_handle_t *)handle));
  TRACE("arguments: handle = %p, status = %d\n", handle, status);
  
  timer = (libuv_timer_t *)handle->data;
  assert(timer != NULL);
  TRACE("libuv_timer_t attributes: uvtimer = %p, L = %p, ref = 0x%012x, ref_count = %d\n", timer->uvtimer, timer->L, timer->ref, timer->ref_count);

  L = timer->L;
  assert(L != NULL);

  /* [ userdata ] */
  /* DUMP_STACK(L); */

  lua_rawgeti(L, LUA_REGISTRYINDEX, timer->ref); /* [ userdata, userdata ] */
  assert(lua_gettop(L) == 2);

  lua_getfenv(L, -1); /* [ userdata, userdata, env_table ] */
  lua_getfield(L, -1, "on_timer"); /* [ userdata, userdata, env_table, function ] */
  if (!lua_isfunction(L, -1)) { /* check handler */
    /* TODO: error */
    fprintf(stderr, "Invalid handler");
    return;
  }

  lua_pushinteger(L, status); /* [ userdata, userdata, env_table, function, number ] */

  lua_call(L, 1, 0);

  timer->ref_count--;
}

static int new_timer (lua_State *L) {
  int ret = 0;
  libuv_timer_t *timer = NULL;
  uv_timer_t *uvtimer = NULL;
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

  /* create libuv_timer_t */
  timer = (libuv_timer_t *)lua_newuserdata(L, sizeof(libuv_timer_t)); /* [ userdata ] */
  if (timer == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create libuv_timer_t (%p)\n", timer);
  assert(timer != NULL);

  /* create uv_timer_t */
  uvtimer = (uv_timer_t *)malloc(sizeof(uv_timer_t));
  if (uvtimer == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create uv_timer_t (%p)\n", uvtimer);
  assert(uvtimer != NULL);

  /* libuv_timer_t settings */
  timer->uvtimer = uvtimer;
  timer->L = L;
  timer->ref = LUA_NOREF;
  timer->ref_count = 0;

  /* set envriment table to userdata */
  lua_newtable(L); /* [ userdata, table ] */
  lua_setfenv(L, -2); /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* set metatable */
  luaL_getmetatable(L, TIMER_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* initialize timer */
  ret = uv_timer_init(loop->uvloop, timer->uvtimer);
  if (ret) {
    /* TODO: error */
    return 0;
  }
  TRACE("uv_timer_init: ret = %d\n", ret);

  /* return userdata */
  return 1;
}

static int delete_timer (lua_State *L) {
  libuv_timer_t *timer = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  timer = (libuv_timer_t *)luaL_checkudata(L, 1, TIMER_T); /* [ userdata ] */
  assert(timer != NULL);
  TRACE("libuv_timer_t attributes: uvtimer = %p, L = %p, ref = 0x%012x, ref_count = %d\n", timer->uvtimer, timer->L, timer->ref, timer->ref_count);

  if (!timer->ref_count & (timer->ref != LUA_NOREF)) {
    TRACE("unreference uv_timer_t\n");
    luaL_unref(L, LUA_REGISTRYINDEX, timer->ref);
    timer->ref = LUA_NOREF;
    timer->uvtimer->data = NULL;
    TRACE("free uv_timer_t\n");
    free(timer->uvtimer);
    timer->uvtimer = NULL;
  }

  if (timer->uvtimer) {
    fprintf(stderr, "WARNING: foget to close\n");
    uv_close(timer->uvtimer, on_close_uv_timer_handle);
  }

  return 0;
}

static int start_timer (lua_State *L) {
  int ret = 0;
  int ref_ret = LUA_REFNIL;
  int64_t repeat = 0;
  int64_t timeout = 0;
  libuv_timer_t *timer = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);
  /* [ userdata, function, number, (boolean) ] */

  /* TODO: check argument count */
  assert(lua_gettop(L) >= 3); 

  /* get repeat argument */
  if (lua_gettop(L) == 4 && lua_isboolean(L, -1)) {
    repeat = (int64_t)lua_tonumber(L, -1);
    lua_pop(L, 1);
    assert(lua_gettop(L) == 3);
  } else {
    repeat = 0;
  }
  TRACE("repeat = %d\n", repeat); 

  /* [ userdata, function, number ] */
  /* get timeout argument */
  timeout = (int64_t)luaL_checklong(L, -1);
  lua_pop(L, 1);
  assert(lua_gettop(L) == 2);

  /* [ userdata, function ] */
  /* get callback argument */
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_getfenv(L, 1); /* [ userdata, function, env_table ] */
  lua_pushvalue(L, 2); /* [ userdata, function, env_table, function ] */
  lua_remove(L, 2); /* [ userdata, env_table, function ] */
  lua_setfield(L, -2, "on_timer"); /* [ userdata, env_table ] */
  lua_pop(L, 1); /* [ userdata ] */

  /* get self argument */
  timer = (libuv_timer_t *)luaL_checkudata(L, 1, TIMER_T);
  TRACE("get libuv_timer_t (%p)\n", timer);
  assert(timer != NULL);
  assert(timer->uvtimer != NULL);
  timer->uvtimer->data = timer;
  assert(timer->uvtimer->data != NULL);

  /* execute uv_timer_start */
  ret = uv_timer_start(timer->uvtimer, on_timer, timeout, repeat);
  TRACE("uv_timer_start: ret = %d\n", ret);

  /* set reference */
  if (!timer->ref_count) {
    ref_ret = luaL_ref(L, LUA_REGISTRYINDEX);
    if (ref_ret == LUA_REFNIL) {
      /* TODO: error */
      fprintf(stderr, "FAIELD luaL_ref !!");
      return 0;
    }
    timer->ref = ref_ret;
  }
  timer->ref_count++;
  assert(timer->ref != LUA_NOREF);

  TRACE("libuv_timer_t attributes: uvtimer = %p, L = %p, ref = 0x%012x, ref_count = %d\n", timer->uvtimer, timer->L, timer->ref, timer->ref_count);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}


static const struct luaL_Reg libuv_timer_methods [] = {
  { "__gc", delete_timer },
  { "start", start_timer },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_timer (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_timer (lua_State *L) {
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
  luaL_newmetatable(L, TIMER_T); /* [ table, metatable ] */
  TRACE("create libuv.timer metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.timer metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_timer_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist timer methods\n");

  /* create Timer table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_timer); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "Timer"); /* [ table ] */
  TRACE("set 'Timer' attribute\n");

  return 0;
}
