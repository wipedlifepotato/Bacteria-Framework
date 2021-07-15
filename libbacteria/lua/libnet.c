#include "libnet.h"
INITLUAFUNC(send) {}
INITLUAFUNC(recv) {}
INITLUAFUNC(join_addresses){
	size_t cAddresses = lua_gettop (L);
//	char * argumets[cAddresses];
	size_t c = 1;
	char * pAddr = NULL;
	for(size_t i = cAddresses;i--;){
		const char * argument = lua_tostring(L, c++);
	
		char * addr;
		if(pAddr == NULL){
		 addr = join_addresses(argument, NULL);
		 pAddr = addr;//f not need
		}
		else{
		 addr = join_addresses(pAddr,argument, NULL);
		 free(pAddr); //f 1
		 pAddr = addr;
		}
	}//
	lua_pushstring(L, pAddr);
	free(pAddr);//f l
	return 1;
}
INITLUAFUNC(unpackData){}

INITLUAFUNC(join_data){
	const char * a = lua_tostring(L, 1);
	const char * b = lua_tostring(L, 2);
	char splitter = ';';
	if( lua_gettop(L) > 2 ) splitter = lua_tostring(L,3)[0]; 
	char * r = join_data(a,b, splitter);
	lua_pushstring(L,r);
	free(r);
	return 1;
}

INITLUAFUNC(toBase64){
	const char * data = lua_tostring(L,1);
	char* base64str;
	Base64Encode((char*)data, strlen(data), &base64str);
	lua_pushstring(L,base64str);
	free(base64str);
	return 1;
	
}
INITLUAFUNC(decBase64){
	const char * data = lua_tostring(L,1);
        unsigned char* base64DecodeOutput;
	size_t sB64Decoded;
	Base64Decode((char*)data, &base64DecodeOutput, &sB64Decoded);
	lua_pushlstring(L,base64DecodeOutput,sB64Decoded);
	free(base64DecodeOutput);
	return 1;
}

int luaopen_lnet(lua_State *L) {
  luaL_openlib(L, "lnet", lnet, 0);
  return 1;
}
