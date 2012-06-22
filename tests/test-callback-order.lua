libuv = require('libuvlua')

local idle_cb_called = 0
local timer_cb_called = 0

local loop = libuv.Loop.default()
assert(loop)

local idle = nil
local timer = nil

local idle_cb = function (_idle, status)
   assert(idle == _idle)
   assert(idle_cb_called == 0)
   assert(timer_cb_called == 0)
   assert(_idle:stop())
   idle_cb_called = idle_cb_called + 1
end

local timer_cb = function (_timer, status)
   assert(timer == _timer)
   assert(idle_cb_called == 1)
   assert(timer_cb_called == 0)
   assert(_timer:stop())
   timer_cb_called = timer_cb_called + 1
end

local next_tick = function (handle, status)
   assert(handle:stop())
   --assert(handle:close() == nil)
   idle = libuv.Idle.new(loop)
   assert(idle)
   assert(idle:start(idle_cb))
   timer = libuv.Timer.new(loop)
   assert(timer)
   assert(timer:start(timer_cb, 1, 0))
end

local idle_first = libuv.Idle.new(loop)
assert(idle_first:start(next_tick))
assert(idle_cb_called == 0)
assert(timer_cb_called == 0)

assert(loop:run())

assert(idle_cb_called == 1)
assert(timer_cb_called == 1)

