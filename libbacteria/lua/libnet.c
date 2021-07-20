#include "libnet.h"

INITLUAFUNC(set_timeout){
	int socket = lua_tonumber(L,1);
	unsigned int tSec = lua_tonumber(L,2);
	unsigned int tUsec = lua_tonumber(L,3);
	set_timeout(socket, tSec, tUsec);
	lua_pushnil(L);
	return 1;
}

INITLUAFUNC(send){
	size_t cTop = lua_gettop(L);
	int sock = lua_tonumber(L,1);
	const unsigned char * msg = lua_tostring(L,2);
	int flags = O_NONBLOCK;
	struct sockaddr * dadr; socklen_t addrlen =sizeof(struct sockaddr_in); 
	if ( sock <= 0 ) luaL_error(L, "Socket is bad");
	if( cTop >= 3 ) dadr = (struct sockaddr*)lua_touserdata(L,3);
	if( cTop >= 4 ) addrlen = lua_tonumber(L, 4);
	if( cTop == 5) flags = flags;
	size_t smsg = strlen(msg);
	int r = dadr == NULL ? send(sock, msg, smsg, flags) : sendto(sock, msg, smsg, flags, dadr, addrlen);
	if( r<=0 ) lua_pushnil(L);
	else lua_pushboolean(L, 1);
	return 1;
}
INITLUAFUNC(recv){
	size_t cTop = lua_gettop(L);
	int sock = lua_tonumber(L,1);
	size_t recv_count = lua_tonumber(L,2);
	int flags = O_NONBLOCK;
	struct sockaddr * dadr; socklen_t addrlen; 
	if ( sock <= 0 ) luaL_error(L, "Socket is bad");
	if( cTop >= 3 ) dadr = (struct sockaddr*)lua_touserdata(L,3);
	if( cTop == 4 ) flags = flags;
	char buf[recv_count+1];
	bzero(buf, recv_count+1);
	size_t r = dadr == NULL ? recv(sock, buf, recv_count, flags) : recvfrom( sock, buf, recv_count, flags, dadr, &addrlen);
	if(r <= 0){
	 lua_pushnil(L);
	 return 1;
	}
	buf[r]=0;
	lua_pushnumber(L, addrlen);
	lua_pushnumber(L, r);
	lua_pushstring(L, buf);
	return 3;
}

INITLUAFUNC(getsockaddr){
  int sin_family = AF_INET;
  uint16_t port = htons( lua_tonumber(L,2) );
  const char * host = lua_tostring(L,1);

  uint32_t s_addr = inet_addr(host);

  if( lua_gettop(L) == 3 ) sin_family = lua_tonumber(L,3);

  size_t nbytes = sizeof(struct sockaddr_in) + sizeof(int)*(sin_family-1) + sizeof(in_port_t)*(port-1) + sizeof(uint32_t)*(s_addr-1);
  struct sockaddr_in *rr = (struct sockaddr_in *)lua_newuserdata(L, nbytes);


  rr->sin_family = sin_family;
  rr->sin_port = port;
  rr->sin_addr.s_addr = s_addr;
  return 1;
}

#define LUABINDADRERR(adr_ptr, TCPUDP)\
 			char err[256];\
			 char *ip = inet_ntoa(adr->sin_addr);\
			 uint16_t port = htons (adr->sin_port);\
			 sprintf(err, "Can't bind %s %s:%d%c", TCPUDP, ip, port, '\0'); \
			 luaL_error(L,err);
INITLUAFUNC(accept){
	int sock = lua_tonumber(L,1);
	if(sock<=0) luaL_error(L, "Bad sock descriptor for accept");
	struct sockaddr_in addr;
	socklen_t addrlen;
	int r = accept(sock, (struct sockaddr*)&addr, &addrlen);
	if(r <= 0){
		 lua_pushnil(L);
		 return 1;
	}
	size_t nbytes = sizeof(struct sockaddr_in) + sizeof(int)*(addr.sin_family-1) 
	+ sizeof(in_port_t)*(addr.sin_port-1) + sizeof(uint32_t)*(addr.sin_addr.s_addr-1);
  	struct sockaddr_in *rr = (struct sockaddr_in *)lua_newuserdata(L, nbytes);
        rr->sin_family = addr.sin_family;
        rr->sin_port = addr.sin_port;
        rr->sin_addr.s_addr = addr.sin_addr.s_addr;
	lua_pushnumber(L, r);
	return 2;
}

INITLUAFUNC(bind){
	size_t cArgs = lua_gettop(L);
	bool useTCP = true;
	bool useUDP = false;
	int sock_tcp=0, sock_udp =0, listen_count=100;
	socklen_t socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in *adr = (struct sockaddr_in*)lua_touserdata(L,1);
	if(adr == NULL) 
		luaL_error(L, "bad bind addr (use lnet.getsockaddr(host,port) as example before [sin_family optional as third argument])");
	if(cArgs >= 2) useTCP = lua_toboolean(L,2);
	if(cArgs >= 3) useUDP = lua_toboolean(L,3);
	if(cArgs >= 4) listen_count = lua_tonumber(L,4);
	if(cArgs >= 5) socklen = lua_tonumber(L,5);
	if(useTCP){
		sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
		int rsock = bind(sock_tcp, (struct sockaddr*)adr, socklen);
		if( rsock <= 0){
			 LUABINDADRERR(adr, "(TCP)");
		}
		listen(sock_tcp, listen_count);
	}
	if(useUDP){
		sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
		int rsock = bind(sock_udp, (struct sockaddr*)adr, socklen);
		if( rsock <= 0){
			 LUABINDADRERR(adr, "(UDP)");
		}
	}
	if(useUDP) lua_pushnumber(L, sock_udp);
	if(useTCP) lua_pushnumber(L, sock_tcp);
	if(useUDP && useTCP) return 2;
	return 1; 
}
INITLUAFUNC(close_sock){
	int sock = lua_tonumber(L,1);
	if(sock <= 0 ) luaL_error(L, "Bad socket for close");
	close(sock);
	lua_pushboolean(L, 1);
	return 1;
}

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

//#define SPLITADDRCHAR ';'
//#define FSPLITTERCHAR '='

INITLUAFUNC(unpackData){
//char ** unpackData(char * data, size_t * rt_size, char fsplitter, char ssplitter){
	const char * packedData = lua_tostring(L,1);
	char ssplitter=SPLITADDRCHAR;
	char fsplitter=FSPLITTERCHAR;

	size_t cArgs = lua_gettop(L);
	if(cArgs >= 2) fsplitter= lua_tostring(L,2)[0];
	if(cArgs >= 3) ssplitter= lua_tostring(L,3)[0];

	size_t rt_size;
	char ** rv = unpackData((char*)packedData, &rt_size, fsplitter, ssplitter);
	if( rv == NULL ) luaL_error(L, "bad packed data");

	lua_newtable(L);
	for(unsigned int i = 0; i< rt_size;i+=2){
		//printf("UNPACKDATA(LUA): %s=%s\n", rv[i], rv[i+1]);
	    lua_pushlstring(L, rv[i], strlen(rv[i]));
	    lua_pushlstring(L, rv[i+1], strlen(rv[i+1]));
	    lua_settable(L, -3);
	}
	free_splitted(rv, rt_size);
	return 1;
}
INITLUAFUNC(packData){
//char * packData(char ** dataKeys, char ** dataValues, char fsplitter, char ssplitter){
	//const char * 
	luaL_checktype(L, 1, LUA_TTABLE);
	//lua_gettable(L, -1); 
//	if ((lua_type(L, -2) == LUA_TSTRING))
	size_t sTable = 1;//lua_objlen(L,1);
	char ** dataKeys = (char**)malloc(sizeof(char*) * sTable+1);
	char ** dataValues = (char**)malloc(sizeof(char*) * sTable+1);
//	printf("DataKeysPtr: %p; dataValuesPtr:%p\n", dataKeys, dataValues);

	if(dataKeys == NULL || dataValues == NULL) luaL_error(L,"Memory out");
	size_t i = 0;
	lua_pushnil(L);
	 while(lua_next(L, -2) != 0) {
	    if(i > sTable){
		sTable+=2;
		dataKeys = (char**) realloc(dataKeys, sizeof(char*) * sTable+1);
		dataValues = (char**) realloc(dataValues, sizeof(char*) * sTable+1);
		if(dataKeys == NULL || dataValues == NULL) luaL_error(L,"Memory out");
//		printf("DataKeysPtr: %p; dataValuesPtr:%p\n", dataKeys, dataValues);
	    }
            if(lua_isstring(L, -1)){
		const char * key = lua_tostring(L, -2);
		const char * value = lua_tostring(L, -1);
		dataKeys[i] = (char*)malloc(sizeof(char) * strlen(key)+1);
		strcpy(dataKeys[i], key);
		dataValues[i] = (char*)malloc(sizeof(char) * strlen(value)+1);
		strcpy(dataValues[i], value);
		i++;
	    }
            else if(lua_isnumber(L, -1)){
		const char * key = lua_tostring(L, -2);
		float value = lua_tonumber(L, -1);
		dataKeys[i] = (char*)malloc(sizeof(char) * strlen(key)+1);
		strcpy(dataKeys[i], key);
		char numstr[256];
		sprintf(numstr, "%f%c", value, '\0');
		dataValues[i] = (char*)malloc(sizeof(char) * strlen(numstr)+1);
		strcpy(dataValues[i], numstr);
		i++;
	    }
            else if(lua_istable(L, -1)) {
                lua_pushnil(L);
            }
            lua_pop(L, 1);
        }
	
	dataKeys[i] = NULL;
	dataValues[i] = NULL;
	char * data = packData(dataKeys,dataValues, FSPLITTERCHAR, SPLITADDRCHAR);
	lua_pushstring(L, data);
	//printf("Data: %s\n", data);
	free(data);
	for(unsigned int z = sTable+1; z--;){
		free( dataKeys[z] );
		free( dataValues[z] );
	}
	free(dataKeys);
	free(dataValues);

	return 1;
}


int luaopen_lnet(lua_State *L) {
  luaL_openlib(L, "lnet", lnet, 0);
  return 1;
}
