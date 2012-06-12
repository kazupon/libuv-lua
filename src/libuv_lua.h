#ifndef LIBUV_LUA_H
#define LIBUV_LUA_H

#ifdef DEBUG
#define TRACE(fmt, ...)  do { fprintf(stderr, "%s: %d: %s: " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)
#else
#define TRACE(fmt, ...)  ((void)0)
#endif /* DEBUG */


//void debug_printf (const char *fmt, ...);

#endif /* LIBUV_LUA_H */
