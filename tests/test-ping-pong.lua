libuv = require('libuvlua')

libuv.hello()

local server_closed = false

local loop = libuv.Loop.default
assert(loop, 'event loop create error !!')

local tcp_server = libuv.Tcp:new(loop)
assert(tcp_server, 'server socket create error !!')

assert(tcp_server:bind(1978, 'localhost'), 'bind error !!')


local on_close = function (handle)
   print('occured onclose event')
end

local after_shutdown = function (handle, status)
   print('occured after shutdown event: status = ' .. tostring(status))
   libuv.close(handle, on_close)
end

local after_write = function (req, status)
   print('occured after write event : status = ' .. tostring(status))

   if status then
      local err = libuv.last_error(loop)
      print(string.format('write error: %s', libuv.strerror(err)))
      assert(false)
   end
end

local echo_alloc = function (handle, suggested_size)
   return libuv.Buffer:new(suggested_size)
end

local after_read = function (handle, nread, buf)
   if nread < 0 then
      assert(libuv.last_error(loop).code == libuv.UV_EOF)
      assert(libuv.shutdown(libuv.Shutdown:new(), handle, after_shutdown), 'shutdown error !!')
   end
   -- check server close packet ??
   if not server_closed then
   end

   local wr = libuv.WriteRequest:new()
   wr:setBuffer(libuv.Buffer:new(buf:getBase(), nread))
   assert(libuv.write(wr:getRequest(), wr:getBuffer(), 1, after_write), 'write error !!')
end

local on_connection = function (tcp_server, status)
   assert(tcp_server, 'tcp_server nothing !!')
   print('on connection : status ' .. tostring(status))
   assert(status == 0)

   local client = libuv.Tcp:new(loop)
   assert(client, 'client socket create error !!')

   client:setData(tcp_server)
   
   assert(libuv.accept(tcp_server, client), 'accept error !!')

   assert(libuv.read_start(client, echo_alloc, after_read), 'read_start error !!')
end

assert(libuv.listen(tcp_server, on_connection), 'listen error !!')

assert(loop:run(), 'event loop run error !!')
