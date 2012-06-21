libuv = require('libuvlua')

local loop1 = libuv.Loop.new()
print('loop1', loop1)
assert(loop1, 'cannot create event loop')

local loop2 = libuv.Loop.new()
print('loop2', loop2)
assert(loop1 ~= loop2, 'equal event loop')

local default_loop1 = libuv.Loop.default()
print('default_loop1', default_loop1)
local default_loop2 = libuv.Loop.default()
print('default_loop2', default_loop2)
assert(default_loop1 == default_loop2, 'not equal default event loop')

assert(loop1:run_once(), 'failed run_once')
assert(default_loop1:run(), 'failed run')

assert(default_loop2:update_time() == nil, 'failed update_time')
assert(loop2:now(), 'failed now')
print('now', loop1:now())

assert(default_loop1:last_err_code() == 0)
assert(default_loop1:last_err_msg() == 'success')

loop1 = nil
loop2 = nil
default_loop1 = nil
default_loop2 = nil

collectgarbage()

