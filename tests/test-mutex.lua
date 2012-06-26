libuv = require('libuvlua')

local mutex = libuv.Mutex.new()
assert(mutex)

assert(mutex:lock() == nil)
assert(mutex:unlock() == nil)

