/*
 * Debugging utilities.
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_debug.h"

#include <stdio.h>


void dump_stack (lua_State *L) {
  int i;
  int top = lua_gettop(L);
  printf("--------------- STACK ---------------\n");
  for (i = 1; i <= top; i++) {
    int t = lua_type(L, i);
    printf("Stack[%2d - %8s] : ", i, lua_typename(L, t));
    switch (t) {
      case LUA_TSTRING:
        printf("%s", lua_tostring(L, i));
        break;
      case LUA_TBOOLEAN:
        printf(lua_toboolean(L, i) ? "true" : "false");
        break;
      case LUA_TNUMBER:
        printf("%g", lua_tonumber(L, i));
        break;
      case LUA_TNIL:
        break;
      default:
        printf("%s", lua_typename(L, t));
        break;
    }
    printf("\n");
  }
  printf("-------------------------------------\n");
}

