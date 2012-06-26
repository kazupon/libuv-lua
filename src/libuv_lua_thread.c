/*
 * thread module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_thread.h"

/* other include(s) */
#include <assert.h>
#include "libuv_lua_loop.h"

/* define(s) */
#define THREAD_T "libuv.thread"

/* variable(s) */


/* function(s) */ 
/* thread function */
static void on_thread (void *arg) {
  lua_State *L = NULL;
  libuv_thread_t *thread = NULL;

  assert(arg != NULL);
  TRACE("arguments: arg = %p\n", arg);
  
  thread = (libuv_thread_t *)arg;
  assert(thread != NULL);
  TRACE("libuv_thread_t attributes: uvthread = %p, L = %p\n", thread->uvthread, thread->L);

  L = thread->L;
  assert(L != NULL);

  luaL_openlibs(L); /* open standard library */

  lua_call(L, 1, 1);

  lua_close(thread->L);
}

static int new_thread (lua_State *L) {
  int ret = 0;
  lua_State *thread_state = NULL;
  libuv_thread_t *thread = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ function, table ] */
  assert(lua_gettop(L) == 2);

  /* create libuv_thread_t */
  thread = (libuv_thread_t *)lua_newuserdata(L, sizeof(libuv_thread_t)); /* [ function, table, userdata ] */
  if (thread == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create libuv_thread_t (%p)\n", thread);
  assert(thread != NULL);

  thread_state = lua_newthread(L); /* [ function, table, userdata, thread ] */
  TRACE("thread_state = %p\n", thread_state);
  assert(thread_state != NULL);

  lua_pushvalue(L, 1);
  lua_pushvalue(L, 2);
  lua_xmove(L, thread_state, 2);

  /* libuv_thread_t settings */
  thread->L = thread_state;

  /* set envriment table to userdata */
  lua_newtable(L); /* [ function, table, userdata, thread, table ] */
  lua_setfenv(L, -3); /* [ function, table, userdata, thread ] */
  /*
  assert(lua_gettop(L) == 3);
  */

  /* set metatable */
  luaL_getmetatable(L, THREAD_T); /* [ function, table, userdata, thread, metatable ] */
  lua_setmetatable(L, -3); /* [ function, table, userdata, thread ] */
  TRACE("set metatable to userdata\n");

  /* create thread */
  ret = uv_thread_create(&thread->uvthread, on_thread, (void *)thread);
  if (ret) {
    /* TODO: error */
    return 0;
  }
  TRACE("uv_thread_create: ret = %d\n", ret);

  lua_pop(L, 1);
  /* return userdata */
  return 1;
}

static int delete_thread (lua_State *L) {
  libuv_thread_t *thread = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  thread = (libuv_thread_t *)luaL_checkudata(L, 1, THREAD_T); /* [ userdata ] */
  assert(thread != NULL);
  TRACE("libuv_thread_t attributes: uvthread = %p, L = %p\n", thread->uvthread, thread->L);

  thread->uvthread = NULL;

  return 0;
}

static int join_thread (lua_State *L) {
  int ret = 0;
  libuv_thread_t *thread = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* get self argument */
  thread = (libuv_thread_t *)luaL_checkudata(L, 1, THREAD_T);
  TRACE("get libuv_thread_t (%p)\n", thread);
  assert(thread != NULL);
  assert(thread->uvthread != NULL);
  assert(thread->L != NULL && L != thread->L);

  /* execute uv_thread_join */
  TRACE("call uv_thread_join\n");
  ret = uv_thread_join(&thread->uvthread);
  TRACE("uv_thread_join: ret = %d\n", ret);

  TRACE("libuv_thread_t attributes: uvthread = %p, L = %p\n", thread->uvthread, thread->L);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static const struct luaL_Reg libuv_thread_methods [] = {
  { "__gc", delete_thread },
  { "join", join_thread },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_thread (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_thread (lua_State *L) {
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
  luaL_newmetatable(L, THREAD_T); /* [ table, metatable ] */
  TRACE("create libuv.thread metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.thread metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_thread_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist thread methods\n");

  /* create thread table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_thread); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "Thread"); /* [ table ] */
  TRACE("set 'Thread' attribute\n");

  return 0;
}
