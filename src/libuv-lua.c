#include <stdio.h>
#include <assert.h>

#include "lua.h"
#include "lauxlib.h"


static int hello (lua_State *L) {
    assert(L != NULL);
    printf("call hello\n");
    return 0;
}

static const struct luaL_Reg libuv_bindings [] = {
    { "hello", hello },
    { NULL, NULL }, /* sentinail */
};

int luaopen_libuvlua (lua_State *L) {
    luaL_register(L, "libuvlua", libuv_bindings);
    return 1;
}

