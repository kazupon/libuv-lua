libuv = require('libuvlua')

local rwlock = libuv.RWLock.new()
assert(rwlock)

assert(rwlock:rdlock() == nil)
assert(rwlock:rdunlock() == nil)
assert(rwlock:wrlock() == nil)
assert(rwlock:wrunlock() == nil)

