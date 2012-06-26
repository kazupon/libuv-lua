/*
 * Task module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_task.h"

/* other include(s) */
#include <assert.h>
#include "mp.h"
#include "libuv_lua_loop.h"

/* define(s) */
#define TASK_T "libuv.task"

/* variable(s) */


/* function(s) */

/* work callback function */
static void on_work (uv_work_t *req) {
  lua_State *L = NULL;
  libuv_task_t *task = NULL;

  assert(req != NULL);
  TRACE("arguments: req = %p\n", req);

  task = (libuv_task_t *)req->data;
  assert(task != NULL);
  TRACE("libuv_task_t attributes: uvtask = %p, loop = %p, L = %p, data = %p, ref = 0x%012x\n", task->uvtask, task->loop, task->L, task->data, task->ref);

  L = task->L;
  assert(L != NULL);

  /* [ userdata ] */
  /* DUMP_STACK(L); */

  lua_rawgeti(L, LUA_REGISTRYINDEX, task->ref); /* [ userdata, userdata ] */
  assert(lua_gettop(L) == 2);

  lua_getfenv(L, -1); /* [ userdata, userdata, env_table ] */
  lua_getfield(L, -1, "on_work"); /* [ userdata, userdata, env_table, function ] */
  if (!lua_isfunction(L, -1)) { /* check handler */
    /* TODO: error */
    fprintf(stderr, "Invalid handler");
    return;
  }

  lua_pushvalue(L, 2); /* [ userdata, userdata, env_table, function, userdata ] */
  lua_call(L, 1, 0);
  lua_settop(L, 1);
}

/* after work callback function */
static void on_after_work (uv_work_t *req) {
  lua_State *L = NULL;
  libuv_task_t *task = NULL;

  assert(req != NULL);
  TRACE("arguments: req = %p\n", req);

  task = (libuv_task_t *)req->data;
  assert(task != NULL);
  TRACE("libuv_task_t attributes: uvtask = %p, loop = %p, L = %p, data = %p, ref = 0x%012x\n", task->uvtask, task->loop, task->L, task->data, task->ref);

  L = task->L;
  assert(L != NULL);

  /* [ userdata ] */
  /* DUMP_STACK(L); */

  lua_rawgeti(L, LUA_REGISTRYINDEX, task->ref); /* [ userdata, userdata ] */
  assert(lua_gettop(L) == 2);

  lua_getfenv(L, -1); /* [ userdata, userdata, env_table ] */
  lua_getfield(L, -1, "on_after_work"); /* [ userdata, userdata, env_table, function ] */
  if (!lua_isfunction(L, -1)) { /* check handler */
    /* TODO: error */
    fprintf(stderr, "Invalid handler");
    return;
  }

  lua_pushvalue(L, 2); /* [ userdata, userdata, env_table, function, userdata ] */
  lua_call(L, 1, 0);
  lua_settop(L, 1);
}

static int new_task (lua_State *L) {
  int ret = 0;
  uv_work_t *uvtask = NULL;
  libuv_task_t *task = NULL;
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

  /* create libuv_task_t */
  task = (libuv_task_t *)lua_newuserdata(L, sizeof(libuv_task_t)); /* [ userdata ] */
  if (task == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create libuv_task_t (%p)\n", task);
  assert(task != NULL);

  /* create uv_work_t */
  uvtask = (uv_work_t *)malloc(sizeof(uv_work_t));
  if (uvtask == NULL) {
    /* TODO: error */
    return 0;
  }
  TRACE("create uv_work_t (%p)\n", uvtask);
  assert(uvtask != NULL);

  /* libuv_task_t settings */
  task->uvtask = uvtask;
  task->uvtask->data = task;
  task->loop = loop->uvloop;
  task->L = L;
  task->data = NULL;
  task->ref = LUA_NOREF;

  /* set envriment table to userdata */
  lua_newtable(L); /* [ userdata, table ] */
  lua_setfenv(L, -2); /* [ userdata ] */
  assert(lua_gettop(L) == 1);

  /* set metatable */
  luaL_getmetatable(L, TASK_T); /* [ userdata, metatable ] */
  lua_setmetatable(L, -2); /* [ userdata ] */
  TRACE("set metatable to userdata\n");

  /* return userdata */
  return 1;
}

static int delete_task (lua_State *L) {
  libuv_task_t *task = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  task = (libuv_task_t *)luaL_checkudata(L, 1, TASK_T); /* [ userdata ] */
  assert(task != NULL);
  TRACE("libuv_task_t attributes: uvtask = %p, loop = %p, L = %p, data = %p, ref = 0x%012x\n", task->uvtask, task->loop, task->L, task->data, task->ref);

  assert(task->uvtask != NULL);
  assert(task->loop != NULL);
  assert(task->ref != LUA_NOREF);

  TRACE("free uv_task_t\n");
  task->uvtask->data = NULL;
  free(task->uvtask);
  task->uvtask = NULL;
  task->loop = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, task->ref);
  task->ref = LUA_NOREF;

  return 0;
}

static int do_work_task (lua_State *L) {
  int ret = 0;
  int ref_ret = LUA_REFNIL;
  libuv_task_t *task = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata, function, function ] */
  assert(lua_gettop(L) == 3); 

  /* get callback argument */
  luaL_checktype(L, 2, LUA_TFUNCTION);
  luaL_checktype(L, 3, LUA_TFUNCTION);
  lua_getfenv(L, 1); /* [ userdata, function, function, env_table ] */
  lua_pushvalue(L, 2); /* [ userdata, function, function, env_table, function ] */
  lua_remove(L, 2); /* [ userdata, function, env_table, function ] */
  lua_setfield(L, -2, "on_work"); /* [ userdata, function, env_table ] */
  lua_pushvalue(L, 2); /* [ userdata, function, env_table, function ] */
  lua_remove(L, 2); /* [ userdata, env_table, function ] */
  lua_setfield(L, -2, "on_after_work"); /* [ userdata, env_table ] */
  lua_pop(L, 1); /* [ userdata ] */

  /* get self argument */
  task = (libuv_task_t *)luaL_checkudata(L, 1, TASK_T);
  TRACE("get libuv_task_t (%p)\n", task);
  assert(task != NULL);
  assert(task->loop != NULL);

  /* set reference */
  TRACE("referencing uv_task_t\n");
  ref_ret = luaL_ref(L, LUA_REGISTRYINDEX);
  if (ref_ret == LUA_REFNIL) {
    /* TODO: error */
    fprintf(stderr, "FAIELD luaL_ref !!");
    return 0;
  }
  task->ref = ref_ret;
  TRACE("libuv_task_t attributes: uvtask = %p, loop = %p, L = %p, data = %p, ref = 0x%012x\n", task->uvtask, task->loop, task->L, task->data, task->ref);

  /* execute uv_queue_work */
  ret = uv_queue_work(task->loop, task->uvtask, on_work, on_after_work);
  TRACE("uv_queue_work: ret = %d\n", ret);

  /* set return */
  lua_pushinteger(L, ret); /* [ userdata, number ] */

  return 1;
}

static int get_data_task (lua_State *L) {
  int ret = 0;
  libuv_task_t *task = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata ] */
  assert(lua_gettop(L) == 1); 

  /* get self argument */
  task = (libuv_task_t *)luaL_checkudata(L, 1, TASK_T);
  TRACE("get libuv_task_t (%p)\n", task);
  assert(task != NULL);
  assert(task->loop != NULL);

  TRACE("libuv_task_t attributes: uvtask = %p, loop = %p, L = %p, data = %p, ref = 0x%012x\n", task->uvtask, task->loop, task->L, task->data, task->ref);

  if (task->data) {
    size_t len = strlen((const char *)task->data);
    TRACE("data = %s, length = %d\n", task->data, len);
    mprbuf_t rb;
    mprbuf_init(&rb, (const unsigned char *)task->data, len);
    mprbuf_unpack_anytype(&rb, L);
    if (rb.ofs > 0 && rb.err == 0) {
      return 1;
    } else {
      lua_pushnil(L);
      return 1;
    }
  } else {
    lua_pushnil(L);
    return 1;
  }
}

static int set_data_task (lua_State *L) {
  int ret = 0;
  size_t len = 0;
  libuv_task_t *task = NULL;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* [ userdata, table ] */
  assert(lua_gettop(L) == 2); 

  /* get self argument */
  task = (libuv_task_t *)luaL_checkudata(L, 1, TASK_T);
  TRACE("get libuv_task_t (%p)\n", task);
  assert(task != NULL);
  assert(task->loop != NULL);

  TRACE("libuv_task_t attributes: uvtask = %p, loop = %p, L = %p, data = %p, ref = 0x%012x\n", task->uvtask, task->loop, task->L, task->data, task->ref);

  mpwbuf_t buf;
  mpwbuf_init(&buf);
  len = mpwbuf_pack_anytype(&buf, L, 2);
  if (len > 0 && buf.err == 0 ) {
    lua_pushlstring(L, (const char *)buf.data, buf.used);
    task->data = (void *)lua_tolstring(L, -1, NULL);
    lua_pop(L, 1);
    TRACE("task->data = %p (%s)\n", task->data, (const char *)task->data);
  }

  return 0;
}

static const struct luaL_Reg libuv_task_methods [] = {
  { "__gc", delete_task },
  { "do_work", do_work_task },
  { "get_data", get_data_task },
  { "set_data", set_data_task },
  { NULL, NULL }, /* sentinail */
};


LUALIB_API int luaopen_libuv_task (lua_State *L) {
  assert(L != NULL);
  
  /* not implemented */
  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_task (lua_State *L) {
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
  luaL_newmetatable(L, TASK_T); /* [ table, metatable ] */
  TRACE("create libuv.task metatable\n");

  /* metatable.__index = metatable */
  lua_pushvalue(L, -1); /* [ table, metatable, metatable ] */
  lua_setfield(L, -2, "__index"); /* [ table, metatable ] */
  TRACE("set libuv.task metatable\n");

  /* regist methods */
  luaL_register(L, NULL, libuv_task_methods); /* [ table, metatable ] */
  lua_pop(L, 1); /* [ table ] */
  TRACE("regist task methods\n");

  /* create task table */
  lua_newtable(L); /* [ table, table ] */

  lua_pushcfunction(L, new_task); /* [ table, table, function ] */
  lua_setfield(L, -2, "new"); /* [ table, table */
  TRACE("set 'new' function\n");

  lua_setfield(L, -2, "Task"); /* [ table ] */
  TRACE("set 'Task' attribute\n");

  return 0;
}
