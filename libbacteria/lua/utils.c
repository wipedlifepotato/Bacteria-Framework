#include"lua/utils.h"
void getparams(lua_State * L, const char params[], va_list ap){
	if(params == NULL) return;
	size_t params_size = strlen(params);

	//va_list ap;
	//va_start(ap, params);

	while(*params){
		switch(*(params++)){
			case 'i':
				lua_pushnumber(L, va_arg(ap, double));
				break;
			case 's':
				lua_pushstring(L,va_arg(ap, char*));
			case 'b':
				lua_pushboolean(L, va_arg(ap, int));
			case 'm':
				LUA_PUSHTABLESTRING(L, va_arg(ap, char*), va_arg(ap, char*));
				break;
			case 'g'://map but number
				LUA_PUSHTABLENUMBER(L, va_arg(ap, char*), va_arg(ap, int));
				break;
			case 'n':
				lua_pushnil(L);// void* v = va_arg(ap, void*);
				break;
			default:
				fprintf(stderr,"GETPERAMS WARNING: undetifned type\n");
				break;
		}
	}

	va_end(ap);
}
