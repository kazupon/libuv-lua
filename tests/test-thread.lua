libuv = require('libuvlua')

local ret = 0
local th = libuv.Thread.new((function (arg)
   print('call closure', arg)
   return arg.a + arg.b
end), { a = 1, b = 2 })
assert(th)

print('join ...')
assert(th:join())
print('... join done')
assert(ret == 3)
