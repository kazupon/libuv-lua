/*
 * Debugging utilities.
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#ifndef LIBUV_LUA_DEBUG_H
#define LIBUV_LUA_DEBUG_H


#ifdef DEBUG
#define TRACE(fmt, ...)     do { fprintf(stderr, "%s: %d: %s: " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)
#define DUMP_STACK(lua)     do { dump_stack(lua); } while (0)
#else
#define TRACE(fmt, ...)     ((void)0)
#define DUMP_STACK(lua)     ((void)0)
#endif /* DEBUG */


#endif /* LIBUV_LUA__DEBUG_H */
