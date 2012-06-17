libuv = require('libuvlua')

local loop = libuv.Loop.default()
assert(loop)

local once_cb_called = 0
local repeat_cb_called = 0

local once_timers = {}
local rpt = libuv.Timer.new(loop)
local never = libuv.Timer.new(loop)


local once_cb = function (once, status)
   print('once callback', status)
   assert(status == 0)
   once_cb_called = once_cb_called + 1
   once:close()
   loop:update_time()
end

local repeat_cb = function (rpt, status)
   print('repeat callback:', status)
   assert(status == 0)
   repeat_cb_called = repeat_cb_called + 1
   if repeat_cb_called == 5 then
      rpt:close()
   end
end

local never_cb = function (never, status)
   print('nerver callback:', status)
   assert(false)
end

local start_time = loop:now()
assert(0 < start_time)

-- Let 10 timers time out in 500 ms total.
for i = 1, 10 do
   once_timers[i] = libuv.Timer.new(loop)
   assert(once_timers[i])
   assert(once_timers[i]:start(once_cb, (i - 1) * 50, 0))
end

-- The 11th timer is a repeating timer that runs 4 times
assert(rpt)
assert(rpt:start(repeat_cb, 100, 100))

-- The 12th timer should not do anything
assert(never)
assert(never:start(never_cb, 100, 100))
assert(never:stop())

assert(loop:run())

assert(once_cb_called == 10)
print('repeat_cb_called ' .. repeat_cb_called)
assert(repeat_cb_called == 5)

assert(500 <= loop:now() - start_time)

