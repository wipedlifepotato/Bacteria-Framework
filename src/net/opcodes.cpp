// extern void event1(const char params[],...);
#include "opcodes.h"
#include <cstddef>
#include <iostream>

extern "C" {
#include "lua/luaserv.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
};

namespace opcode {
namespace events{

void event1(lua_State *L, int sock, const char *uIp, uint16_t uPort, char *buf,
            ...) {
  puts("event1 -"); //
  PREINIT_CALLLUAEVENFUNC("event1");
  CALLLUAFUNC(4,1);
  GETRETDATA;

  if (send(sock, retdata, strlen((char *)retdata), MSG_NOSIGNAL) < 0) {
    perror("Write error");
  }
}

void event0(lua_State *L, int sock, const char *uIp, uint16_t uPort, char *buf,
            ...) {
  puts("event0 - ");
}
}
} // namespace opcode
