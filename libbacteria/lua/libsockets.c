#include "libsockets.h"
INITLUAFUNC(send) {}
INITLUAFUNC(recv) {}

int luaopen_lsocket(lua_State *L) {
  luaL_openlib(L, "lsocket", lsocket, 0);
  return 1;
}
