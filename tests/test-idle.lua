libuv = require('libuvlua')

local idle_cb_called = 0
local timer_cb_called = 0

local loop = libuv.Loop.default()
assert(loop)

local timer = libuv.Timer.new(loop)
assert(timer)

local idle = libuv.Idle.new(loop)
assert(idle)

local timer_cb = function (_timer, status)
   assert(timer == _timer)
   assert(status == 0)
   assert(idle:close() == nil)
   assert(timer:close() == nil)
   timer_cb_called = timer_cb_called + 1
   print(string.format('timer_cb %d', timer_cb_called))
end

local idle_cb = function (_idle, status)
   assert(idle == _idle)
   assert(status == 0)
   idle_cb_called = idle_cb_called + 1
   print(string.format('idle_cb %d', idle_cb_called))
end

assert(idle:start(idle_cb))
assert(timer:start(timer_cb, 50, 0))

assert(loop:run())

assert(idle_cb_called > 0)
assert(timer_cb_called == 1)

