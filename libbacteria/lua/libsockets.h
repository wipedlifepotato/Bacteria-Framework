#include"../net.h"
#include"macros.h"
#include"lua.h"

INITLUAFUNC(send);
INITLUAFUNC(recv);

static const struct luaL_reg lsocket [] = {
      LUAPAIR(send)
      LUAPAIR(recv)
      {NULL, NULL}
};
int luaopen_lsocket (lua_State *L);
