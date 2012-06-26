{
  'variables': {
    'libuvlua_shared_lua%': 'true',
  },

  'targets': [{
    'target_name': 'libuvlua',
    'type': 'shared_library',
    'dependencies': [
      'deps/libuv/uv.gyp:uv',
    ],
    'export_dependent_settings': [
      'deps/libuv/uv.gyp:uv',
    ],
    'sources': [
      'common.gypi',
      'src/mp.c',
      'src/libuv_lua_debug.c',
      'src/libuv_lua_utils.c',
      'src/libuv_lua_mutex.c',
      'src/libuv_lua_rwlock.c',
      'src/libuv_lua_thread.c',
      'src/libuv_lua_loop.c',
      'src/libuv_lua_errors.c',
      'src/libuv_lua_timer.c',
      'src/libuv_lua_idle.c',
      'src/libuv_lua_check.c',
      'src/libuv_lua_prepare.c',
      'src/libuv_lua_task.c',
      'src/libuv_lua.c',
    ],
    'defines': [
      'LIBUV_VERSION="<!(git --git-dir deps/libuv/.git describe --all --tags --always --long)"',
    ],
    'conditions': [
      [
        'libuvlua_shared_lua=="true"', {
          'sources': [
            '<(libuvlua_shared_lua_includes)/lua.h',
            '<(libuvlua_shared_lua_includes)/lauxlib.h',
          ],
        }, {
          'sources': [
            'deps/lua/include/lua.h',
            'deps/lua/include/lauxlib.h',
          ],
        }
      ], [
        'OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
          'cflags': [ '--std=c89' ],
          'defines': [ '_GNU_SOURCE' ]
        }
      ], [
        'OS=="mac"', {
          'direct_dependent_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
            ],
          },
          'defines': [
            '_DARWIN_USE_64_BIT_INODE=1',
          ],
        }
      ]
    ],
    'include_dirs': [
      'src',
      'deps/libuv/include',
    ],
    'direct_dependent_settings': {
      'include_dirs': [
        'src',
        'deps/libuv/include',
      ]
    },
  }, {
    'target_name': 'test',
    'type': 'none',
    'dependencies': [
      'libuvlua'
    ],
    'copies': [
      {
        'destination': '<(PRODUCT_DIR)',
        'files': [
          'tests/test-mutex.lua',
          'tests/test-rwlock.lua',
          'tests/test-thread.lua',
          'tests/test-loop.lua',
          'tests/test-error-codes.lua',
          'tests/test-timer.lua',
          'tests/test-timer-twice.lua',
          'tests/test-timer-multi.lua',
          'tests/test-timer-again.lua',
          'tests/test-idle.lua',
          'tests/test-callback-order.lua',
          'tests/test-loop-handlers.lua',
          'tests/test-thread-pool.lua',
          'tests/test-ping-pong.lua',
        ],
      },
    ],
  }],
}
