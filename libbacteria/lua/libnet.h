#include"../net.h"
#include"macros.h"
#include"lua.h"


INITLUAFUNC(send);
INITLUAFUNC(recv);
INITLUAFUNC(join_addresses);
INITLUAFUNC(join_data);
INITLUAFUNC(toBase64);
INITLUAFUNC(decBase64);

INITLUAFUNC(unpackData);

static const struct luaL_reg lnet [] = {
      LUAPAIR(send)
      LUAPAIR(recv)
      LUAPAIR(join_addresses)
      LUAPAIR(join_data)
      LUAPAIR(toBase64)
      LUAPAIR(decBase64)
      LUAPAIR(unpackData)
      {NULL, NULL}
};
int luaopen_lnet (lua_State *L);
