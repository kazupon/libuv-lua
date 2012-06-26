libuv = require('libuvlua')

local work_cb_count = 0
local after_work_cb_count = 0
local data = { hoge = 1, foo = 'hello' }

local loop = libuv.Loop.default()
assert(loop)

local task = libuv.Task.new(loop)
assert(task)
assert(task:set_data(data) == nil)

local work_cb = function (_task)
   print('work_cb')
   assert(task == _task)
   local t = _task:get_data()
   assert(t)
   assert(data.hoge == t.hoge)
   assert(data.foo == t.foo)
   work_cb_count = work_cb_count + 1
end

local after_work_cb = function (_task)
   print('after_work_cb')
   assert(task == _task)
   local t = _task:get_data()
   assert(t)
   assert(data.hoge == t.hoge)
   assert(data.foo == t.foo)
   after_work_cb_count = after_work_cb_count + 1
end

assert(task:do_work(work_cb, after_work_cb))
assert(loop:run())

assert(work_cb_count == 1)
assert(after_work_cb_count == 1)

