libuv = require('libuvlua')

local loop = libuv.Loop.default()
assert(loop)

local once_cb_called = 0

local once_cb = function (once, status)
   print('once callback', status)
   assert(status == 0)
   once_cb_called = once_cb_called + 1
   once:close()
end

local once = libuv.Timer.new(loop)
assert(once, 'cannot create timer')

local ret = once:start(once_cb, 10, 0)
assert(ret == 0)

assert(loop:run(), 'faild loop:run')

assert(once_cb_called == 1)

