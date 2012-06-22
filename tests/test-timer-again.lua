libuv = require('libuvlua')

local loop = libuv.Loop.default()
assert(loop)

local start_time = loop:now()
assert(start_time > 0)

local timer1 = libuv.Timer.new(loop)
assert(timer1)
local timer2 = libuv.Timer.new(loop)
assert(timer2)
local timer3 = libuv.Timer.new(loop)
assert(timer3)

local repeat1_cb_called = 0
local repeat2_cb_called = 0
local repeat2_cb_allowed = false

local repeat1_cb = function (timer, status)
   assert(timer == timer2)
   assert(status == 0)
   assert(timer:get_repeat() == 50)

   print(string.format('repeat1_cb called after %d ms', loop:now() - start_time))

   repeat1_cb_called = repeat1_cb_called + 1

   assert(timer:again(timer3) == 0)
   if loop:now() >= (start_time + 500) then
      assert(timer:close() == nil)
      -- We're not calling uv_timer_again on repeat2 any more, so after this
      -- timer2_cb is expected
      repeat2_cb_allowed = true
   end
end

local repeat2_cb = function (timer, status)
   assert(timer == timer3)
   assert(status == 0)
   assert(repeat2_cb_allowed)

   print(string.format('repeat2_cb called after %d ms', loop:now() - start_time))

   repeat2_cb_called = repeat2_cb_called + 1

   if timer:get_repeat() == 0 then
      assert(timer:close() == nil)
      return
   end

   print(string.format('timer:get_repeat() = %d ms', timer:get_repeat()))
   assert(timer:get_repeat() == 100)

   -- This shouldn't take effect immediately.
   timer:set_repeat(0)
end


assert(timer1:again(timer1) == -1)
assert(loop:last_err_code() == libuv.Errors.EINVAL)
assert(timer1:close() == nil)

assert(timer2:start(repeat1_cb, 50, 0))
assert(timer2:get_repeat() == 0)
assert(timer2:set_repeat(50) == nil)
assert(timer2:get_repeat() == 50)

assert(timer3:start(repeat2_cb, 100, 100))
assert(timer3:get_repeat() == 100)

assert(loop:run())

assert(repeat1_cb_called == 10)
assert(repeat2_cb_called == 2)

print(string.format('Test took %d ms (expected ~700 ms)', loop:now() - start_time))
assert(700 <= (loop:now() - start_time))

