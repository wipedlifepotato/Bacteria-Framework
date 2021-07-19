#include"../net.h"
#include"macros.h"
#include"lua.h"

INITLUAFUNC(getsockaddr);
INITLUAFUNC(send);
INITLUAFUNC(recv);
INITLUAFUNC(bind);
INITLUAFUNC(accept);
INITLUAFUNC(close_sock);

INITLUAFUNC(join_addresses);
INITLUAFUNC(join_data);

INITLUAFUNC(toBase64);
INITLUAFUNC(decBase64);

INITLUAFUNC(unpackData);
INITLUAFUNC(packData);

INITLUAFUNC(set_timeout);

static const struct luaL_reg lnet [] = {
      LUAPAIR(send)
      LUAPAIR(recv)
      LUAPAIR(set_timeout)
      LUAPAIR(bind)
      LUAPAIR(accept)
      LUAPAIR(close_sock)
      LUAPAIR(getsockaddr)

      LUAPAIR(join_addresses)
      LUAPAIR(join_data)
      LUAPAIR(toBase64)
      LUAPAIR(decBase64)
      LUAPAIR(unpackData)
      LUAPAIR(packData)


      {NULL, NULL}
};
int luaopen_lnet (lua_State *L);
