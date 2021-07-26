#ifndef OPCODES_H
#define OPCODES_H
#include"events.h"
#include<string>

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

//extern void event1(const char params[],...);
namespace opcode{
	constexpr short def_opcode_size = 4;
	using def_opcode_type = char;

	constexpr auto ignorebyte = 0x0E;
	constexpr auto splitbyte = 0x0F;
	using eventfun = events::fun;

	template<typename T, size_t s>
	class opcode_data_basic:public std::array<T,s>{
		opcode_data_basic operator ()(std::array<T,s> && d){
			return opcode_data_basic<T,s>{d};
		}
	};
	

using opcode_data = opcode_data_basic<char,4>;
template<typename Td, size_t s>
class opcode_basic{
	protected:

	protected:
		opcode_data_basic<Td,s> m_data;
		events::event m_event;
	public:
		bool operator==(opcode_basic& a) noexcept{
			for(unsigned short i = 0; i < s; i++){
				if(a.m_data[i] != m_data[i]) return false;
			}
			return true;
		}

		template<typename T>
		bool operator==(T const a) noexcept{
			for(unsigned short i = 0; i < s; i++){
				if(a[i] != m_data[i]) return false;
			}
			return true;
		}
		template<typename T>
		opcode_basic& operator=(T a[]) noexcept{
			unsigned long i =0;
			for(auto b : a){
				m_data[i++]=b;
			}
			return *this;
		}
		opcode_basic(opcode_data_basic<Td,s> data, eventfun fun):
		m_data(data), m_event(fun)
		{}
		events::event& getEvent(void) { return m_event; }



};
using opcode = opcode_basic<def_opcode_type,def_opcode_size>;

}

// events
namespace opcode{
namespace events{
template <typename T,size_t s>
void notFound(lua_State * L, int sock, const char * uIp, uint16_t uPort, char* buf, opcode_data_basic<T,s> op){
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

	void event1(lua_State * L, int sock, const char * uIp, uint16_t uPort, char* buf, ...);
	void event0(lua_State * L, int sock, const char * uIp, uint16_t uPort, char* buf, ...);
/*servs*/
	void getservers(EVENTDEF, ...);
	void addserver(EVENTDEF, ...);
// TODO: 
}
};
#endif
