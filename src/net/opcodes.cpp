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

#define PREINIT_CALLLUAEVENFUNC(namefunc)\
  lua_getglobal(L, namefunc);\
  lua_pushnumber(L, sock);\
  lua_pushstring(L, uIp);\
  lua_pushnumber(L, uPort);\
  lua_pushstring(L, (buf + 4));

#define CALLLUAFUNC(countargs, countresults)\
	  if (lua_pcall(L, /*args*/ countargs, /*results*/ countresults, 0) != 0)\
	  luaL_error(L, "error running function': %s", lua_tostring(L, -1));

#define GETRETDATA\
  unsigned char retdata[512];\
  retdata[0]=0;\
  if(!lua_isnil(L,-1)){\
  if (!lua_isnumber(L, -1)) {\
    printf("(check retval[lua]) Ret val: %s\n", lua_tostring(L, -1));\
    sprintf((char *)retdata, "%s", lua_tostring(L, -1));\
  } else {\
    sprintf((char *)retdata, "%d", (int)lua_tonumber(L, -1));\
    printf("(check retval[lua]) Ret val (num): %d\n", (int)lua_tonumber(L, -1));\
  }}\
  lua_pop(L, 1);

#define SENDRETDATA\
  if(retdata != NULL && strlen((char*)retdata))\
  if (send(sock, retdata, strlen((char *)retdata), MSG_NOSIGNAL) < 0) {\
    perror("Write error");\
  }

namespace opcode {

void notFound(lua_State * L, int sock, const char * uIp, uint16_t uPort, char* buf, opcode_data op){
	PREINIT_CALLLUAEVENFUNC("undefined_event");
	std::string opcodestr;
	for( auto el : op ){
		opcodestr+=el;
	}
	lua_pushstring(L, opcodestr.c_str());
	CALLLUAFUNC(5, 1);
	GETRETDATA;
	SENDRETDATA;

}
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

} // namespace opcode
