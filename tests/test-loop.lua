libuv = require('libuvlua')

local loop1 = libuv.Loop.new()
assert(loop1, 'cannot create event loop')

local loop2 = libuv.Loop.new()
assert(loop1 ~= loop2, 'equal event loop')

local default_loop1 = libuv.Loop.get_default()
local default_loop2 = libuv.Loop.get_default()
assert(default_loop1 == default_loop2, 'not equal default event loop')

loop1 = nil
loop2 = nil
default_loop1 = nil
default_loop2 = nil

collectgarbage()

