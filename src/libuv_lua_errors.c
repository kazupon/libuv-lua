/*
 * Error module
 * Copyright (C) 2012 kazuya kawaguchi. See Copyright Notice in libuv_lua.h
 */

#include "libuv_lua_errors.h"

/* other include(s) */
#include <assert.h>

/* define(s) */


/* variable(s) */


/* function(s) */

LUALIB_API int luaopen_libuv_errors (lua_State *L) {

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  assert(0);

  return 1;
}

LUALIB_API int luaopenL_libuv_errors (lua_State *L) {
  int size = 0;

  TRACE("arguments: L = %p\n", L);
  assert(L != NULL);

  /* check stack size & type check ([ table ]) */
  size = lua_gettop(L);
  if (size != 1 || !lua_istable(L, 1)) {
    return 1;
  }
  assert(size == 1);

  /* create Errors table */
  lua_newtable(L); /* [ table, table ] */

  set_field_integer(L, "UNKNOWN", UV_UNKNOWN);
  set_field_integer(L, "OK", UV_OK);
  set_field_integer(L, "EOF", UV_EOF);
  set_field_integer(L, "EADDRINFO", UV_EADDRINFO);
  set_field_integer(L, "EACCES", UV_EACCES);
  set_field_integer(L, "EAGAIN", UV_EAGAIN);
  set_field_integer(L, "EADDRINUSE", UV_EADDRINUSE);
  set_field_integer(L, "EADDRNOTAVAIL", UV_EADDRNOTAVAIL);
  set_field_integer(L, "EAFNOSUPPORT", UV_EAFNOSUPPORT);
  set_field_integer(L, "EALREADY", UV_EALREADY);
  set_field_integer(L, "EBADF", UV_EBADF);
  set_field_integer(L, "EBUSY", UV_EBUSY);
  set_field_integer(L, "ECONNABORTED", UV_ECONNABORTED);
  set_field_integer(L, "ECONNREFUSED", UV_ECONNREFUSED);
  set_field_integer(L, "ECONNRESET", UV_ECONNRESET);
  set_field_integer(L, "EDESTADDRREQ", UV_EDESTADDRREQ);
  set_field_integer(L, "EFAULT", UV_EFAULT);
  set_field_integer(L, "EHOSTUNREACH", UV_EHOSTUNREACH);
  set_field_integer(L, "EINTR", UV_EINTR);
  set_field_integer(L, "EINVAL", UV_EINVAL);
  set_field_integer(L, "EISCONN", UV_EISCONN);
  set_field_integer(L, "EMFILE", UV_EMFILE);
  set_field_integer(L, "EMSGSIZE", UV_EMSGSIZE);
  set_field_integer(L, "ENETDOWN", UV_ENETDOWN);
  set_field_integer(L, "ENETUNREACH", UV_ENETUNREACH);
  set_field_integer(L, "ENFILE", UV_ENFILE);
  set_field_integer(L, "ENOBUFS", UV_ENOBUFS);
  set_field_integer(L, "ENOMEM", UV_ENOMEM);
  set_field_integer(L, "ENOTDIR", UV_ENOTDIR);
  set_field_integer(L, "EISDIR", UV_EISDIR);
  set_field_integer(L, "ENONET", UV_ENONET);
  set_field_integer(L, "ENOTCONN", UV_ENOTCONN);
  set_field_integer(L, "ENOTSOCK", UV_ENOTSOCK);
  set_field_integer(L, "ENOTSUP", UV_ENOTSUP);
  set_field_integer(L, "ENOENT", UV_ENOENT);
  set_field_integer(L, "ENOSYS", UV_ENOSYS);
  set_field_integer(L, "EPIPE", UV_EPIPE);
  set_field_integer(L, "EPROTO", UV_EPROTO);
  set_field_integer(L, "EPROTONOSUPPORT", UV_EPROTONOSUPPORT);
  set_field_integer(L, "EPROTOTYPE", UV_EPROTOTYPE);
  set_field_integer(L, "ETIMEDOUT", UV_ETIMEDOUT);
  set_field_integer(L, "ECHARSET", UV_ECHARSET);
  set_field_integer(L, "EAIFAMNOSUPPORT", UV_EAIFAMNOSUPPORT);
  set_field_integer(L, "EAISERVICE", UV_EAISERVICE);
  set_field_integer(L, "EAISOCKTYPE", UV_EAISOCKTYPE);
  set_field_integer(L, "ESHUTDOWN", UV_ESHUTDOWN);
  set_field_integer(L, "EEXIST", UV_EEXIST);
  set_field_integer(L, "ESRCH", UV_ESRCH);
  set_field_integer(L, "ENAMETOOLONG", UV_ENAMETOOLONG);
  set_field_integer(L, "EPERM", UV_EPERM);
  set_field_integer(L, "ELOOP", UV_ELOOP);
  set_field_integer(L, "EXDEV", UV_EXDEV);
  set_field_integer(L, "ENOTEMPTY", UV_ENOTEMPTY);
  set_field_integer(L, "ENOSPC", UV_ENOSPC);
  set_field_integer(L, "EIO", UV_EIO);
  set_field_integer(L, "EROFS", UV_EROFS);

  lua_setfield(L, -2, "Errors"); /* [ table ] */
  TRACE("set 'Errors' attribute\n");

  return 0;
}
