# libuv-lua
libuv lua binding


# Requirements

- lua >= 5.1.3
- python >= 2.6


# Building

    $ ./configure --shared-lua --shared-lua-includes=SHARED_LUA_INCLUDES --shared-lua-libpath=SHARED_LUA_LIBPATH --arch=ARCH
    $ make -C out

## Building Options

   $ ./configure --help


# License

## libuv-lua
[MIT license](http://www.opensource.org/licenses/mit-license.php).

## libuv
See deps/libuv/LICENSE file.
