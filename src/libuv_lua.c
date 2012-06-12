#include "libuv_lua.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "lua.h"
#include "lauxlib.h"


void debug_printf (const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

static int hello (lua_State *L) {
    assert(L != NULL);
    TRACE("%s %d\n", "hello", 1);
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

