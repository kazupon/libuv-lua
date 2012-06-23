libuv = require('libuvlua')

IDLE_COUNT = 7
ITERATIONS = 21
TIMEOUT = 1

local loop_iteration = 0
local prepare1_cb_called = 0
local prepare2_cb_called = 0
local check_cb_called = 0
local idle1_cb_called = 0
local idle1_active = 0
local idle2_cb_called = 0
local idle2_cb_started = 0
local idle2_is_active = false

local prepare1 = nil
local prepare2 = nil
local check = nil
local timer = nil
local idle1 = {}
local idle2 = nil

local loop = libuv.Loop.default()
assert(loop)

local timer_cb = function (_timer, status)
   print('timer_cb', loop:now())
   assert(timer == _timer)
   assert(status == 0)
   if loop_iteration == ITERATIONS then
      assert(_timer:close() == nil)
   end
end

local idle2_cb = function (_idle, status)
   print('idle2_cb')
   assert(idle2 == _idle)
   assert(status == 0)
   idle2_cb_called = idle2_cb_called + 1
   assert(_idle:close() == nil)
   idle2_is_active = false
   idle2 = nil
end

local idle1_cb = function (_idle, status)
   print('idle1_cb')
   assert(_idle)
   assert(status == 0)
   assert(idle1_active > 0)
   if (not idle2_is_active and not idle2) then
      idle2 = libuv.Idle.new(loop)
      assert(idle2)
      assert(idle2:start(idle2_cb))
      idle2_is_active = true
      idle2_cb_started = idle2_cb_started + 1
   end
   idle1_cb_called = idle1_cb_called + 1
   if (idle1_cb_called % 5 == 0) then
      assert(_idle:stop())
      idle1_active = idle1_active - 1
   end
end

local check_cb = function (_check, status)
   print('check_cb')
   assert(check == _check)
   assert(status == 0)
   if loop_iteration < ITERATIONS then
      local ite_max = (loop_iteration % IDLE_COUNT) + 1
      for i = 1, ite_max do
         assert(idle1[i]:start(idle1_cb))
         idle1_active = idle1_active + 1
      end
   else
      assert(prepare1:close() == nil)
      assert(check:close() == nil)
      assert(prepare2:close() == nil)
      for i = 1, IDLE_COUNT do
         assert(idle1[i]:close() == nil)
      end
      if idle2_is_active then
         assert(idle2:close() == nil)
         idle2 = nil
         idle2_is_active = false
      end
   end
   check_cb_called = check_cb_called + 1
end

local prepare2_cb = function (_prepare, status)
   print('prepare2_cb')
   assert(prepare2 == _prepare)
   assert(status == 0)
   assert(loop_iteration % 2 ~= 0)
   assert(_prepare:stop())
   prepare2_cb_called = prepare2_cb_called + 1
end

local prepare1_cb = function (_prepare, status)
   print('prepare1_cb')
   assert(prepare1 == _prepare)
   assert(status == 0)
   if (loop_iteration % 2 == 0) then
      assert(prepare2:start(prepare2_cb))
   end
   prepare1_cb_called = prepare1_cb_called + 1
   loop_iteration = loop_iteration + 1
   print(string.format('Loop iteration %d of %d.', loop_iteration, ITERATIONS))
end


prepare1 = libuv.Prepare.new(loop)
assert(prepare1)
assert(prepare1:start(prepare1_cb))

check = libuv.Check.new(loop)
assert(check)
assert(check:start(check_cb))

prepare2 = libuv.Prepare.new(loop)
assert(prepare2)

for i = 1, IDLE_COUNT do
   local idle = libuv.Idle.new(loop)
   assert(idle)
   idle1[i] = idle
end

timer = libuv.Timer.new(loop)
assert(timer)
assert(timer:start(timer_cb, TIMEOUT, TIMEOUT))

assert(loop:run())

assert(loop_iteration == ITERATIONS)
assert(prepare1_cb_called == ITERATIONS)
assert(prepare2_cb_called == math.floor(ITERATIONS / 2.0))
assert(check_cb_called == ITERATIONS)
assert(not idle2_is_active)

