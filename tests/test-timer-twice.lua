libuv = require('libuvlua')

local loop = libuv.Loop.new()
assert(loop)

local once_cb_called = 0

local once_cb = function (status)
   print('once callback', status)
   assert(status == 0)
   once_cb_called = once_cb_called + 1
end

local never_cb = function (staus)
   print('nerver callback:', status)
   assert(false)
end

local once = libuv.Timer.new(loop)
assert(once, 'cannot create timer')

local ret
ret = once:start(never_cb, 86400 * 1000, false)
assert(ret == 0)

ret = once:start(once_cb, 10, false)
assert(ret == 0)

assert(loop:run(), 'faild loop:run')

assert(once_cb_called == 1)

