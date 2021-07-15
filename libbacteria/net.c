#include "net.h"

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

void set_timeout(int socket, unsigned int tSec, unsigned int tUsec) {
  struct timeval tv;
  tv.tv_sec = tSec;
  tv.tv_usec = tUsec;
  setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
}

struct triad_keys init_self_keys(const char *rsa_key_file,
                                 const char *ed25519_key_file,
                                 const char *x25519_privkey) {
  struct triad_keys rt;
  rt.ed25519 = ed25519rsa_initPrivKey(ed25519_key_file, ed25519);
  rt.rsa = ed25519rsa_initPrivKey(rsa_key_file, aRSA);
  rt.x25519 = x25519_initKeyPairFromFile(x25519_privkey);
  if (rt.ed25519.privKey == NULL || rt.rsa.privKey == NULL ||
      rt.x25519.privKey == NULL) {
    fprintf(stderr, "can't init some of keys\n");
    FREEKEYPAIR(rt.ed25519);
    FREEKEYPAIR(rt.rsa);
    if (rt.x25519.privKey != NULL)
      x25519_freeKeyPair(&rt.x25519);
    struct triad_keys null;
    return null;
  }
  return rt;
}

void peer_freeKeys(struct triad_keys *keys) { // selfkeys pub+sec
  FREEKEYPAIR((keys->ed25519));
  FREEKEYPAIR((keys->rsa));
  if (keys->x25519.privKey != NULL)
    x25519_freeKeyPair(&keys->x25519);
}

struct triad_keys generateSelfPeerKeys(const char *ed25519file,
                                       const char *rsafile,
                                       const char *x25519file) {
  struct triad_keys rt;

  FILE *exitFileEd25519 =
      fopen(ed25519file == NULL ? "ed25519.key" : ed25519file, "wb");
  if (exitFileEd25519 == NULL) {
    fprintf(stderr, "Can't open exit file ed25519\n");
    return rt;
  }
  FILE *exitFileRSA = fopen(rsafile == NULL ? "rsa.key" : rsafile, "wb");
  if (exitFileRSA == NULL) {
    fclose(exitFileRSA);
    fprintf(stderr, "Can't open exitFileRSA\n");
    return rt;
  }

  rt.x25519 = x25519_generateKeyPair();

  int r = x25519_savePrivKey(x25519file == NULL ? "x25519.key" : x25519file,
                             &rt.x25519);
  // is can be stealed from RAM. is because is will be readed every time now and
  // now.
  if (r == -1) {
    fprintf(stderr, "Can't save x25519 priv key to %s\n", x25519file);
    peer_freeKeys(&rt);
    fclose(exitFileRSA);
    fclose(exitFileEd25519);
    return rt;
  }

  rt.ed25519 = generateKeysEd25519(exitFileEd25519);
  rt.rsa = generateKeysRSA(RSAKEYSIZE, RSAPRIMARYCOUNT, exitFileRSA);

  fclose(exitFileEd25519);
  fclose(exitFileRSA);

  if (rt.ed25519.privKey == NULL || rt.rsa.privKey == NULL) {
    struct triad_keys nullrt;
    peer_freeKeys(&rt);
    return nullrt;
  }
  return rt;
}

void free_peer(struct peer *p) {
  FREEISNOTNULL(p->host);
  FREEISNOTNULL(p->ed25519_key);
  FREEISNOTNULL(p->x25519_key);
  FREEISNOTNULL(p->rsa_key);
  FREEISNOTNULL(p->identificator);
  FREEISNOTNULL(p->shared_key);
  for (unsigned int i = 0; i < p->host_mirrors_count; i++) {
    FREEISNOTNULL(p->host_mirrors[i]);
  }
  FREEISNOTNULL(p->host_mirrors);
}

struct peer init_self_peer(char *host, uint16_t port, contype allow_con,
                           peertype type, size_t mirrors_count, char *mirrors[],
                           struct triad_keys *keys) {
  struct peer rt;
  assert(allow_con & CON_TCP == CON_TCP || allow_con & CON_UDP == CON_UDP);

  if (allow_con & CON_TCP == CON_TCP)
    rt.sock_tcp = socket(AF_INET, SOCK_STREAM, 0);

  if (allow_con & CON_TCP == CON_UDP)
    *rt.sock_udp = socket(AF_INET, SOCK_DGRAM, 0);

  if ((rt.sock_tcp <= 0 && allow_con & CON_TCP == CON_TCP) ||
      (*rt.sock_udp <= 0 && allow_con & CON_UDP == CON_UDP)) {
    if (rt.sock_tcp > 0)
      close(rt.sock_tcp);
    if (*rt.sock_udp > 0)
      close(*rt.sock_udp);
    fprintf(stderr, "Can't init sockets\n");
    return rt;
  }
  struct sockaddr_in my_addr;
  socklen_t addr_l = sizeof(my_addr);
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = inet_addr(host);

  int ret;
  if (allow_con & CON_TCP == CON_TCP) {
    ret = bind(rt.sock_tcp, (struct sockaddr *)&my_addr,
               sizeof(struct sockaddr_in));
    if (ret == -1) {

      doExit("Can't init bind on (TCP) %s:%d\n", host, port);
    }
    if (listen(rt.sock_tcp, MAX_LISTEN) == -1)
      doExit("Cant start listening (TCP) \n");
  }

  if (allow_con & CON_UDP == CON_UDP) {
    ret = bind(*rt.sock_udp, (struct sockaddr *)&my_addr,
               sizeof(struct sockaddr_in));
    if (ret == -1) {
      doExit("Can't init bind on (UDP) %s:%d\n", host, port);
    }
  }

  rt.addr_in = my_addr;
  rt.port = port;
  size_t host_s = strlen(host);
  rt.host = malloc(sizeof(char) * host_s + 1);
  memcpy(rt.host, host, host_s);
  host[host_s] = 0;
  rt.allow_con = allow_con;
  rt.type = type;
 // rt.isSelf = true;

  size_t sed25519 = strlen(keys->ed25519.pubKey);
  size_t srsa = strlen(keys->rsa.pubKey);

  rt.ed25519_key = malloc(sizeof(char) * sed25519 + 1);
  memcpy(rt.ed25519_key, keys->ed25519.pubKey, sed25519);
  rt.ed25519_key[sed25519] = 0;

  rt.rsa_key = malloc(sizeof(char) * srsa + 1);
  memcpy(rt.rsa_key, keys->rsa.pubKey, srsa);
  rt.rsa_key[srsa] = 0;

  rt.x25519_key = malloc(sizeof(char) * strlen(keys->x25519.pubKey) + 1);
  memcpy(rt.x25519_key, keys->x25519.pubKey, 32);
  rt.x25519_key[32] = 0;

  char hashOfKey[SHA256_OUTPUTSTRING_SIZE];
  toSHA256(rt.x25519_key, hashOfKey);
  rt.identificator = malloc(sizeof(char) * SHA256_OUTPUTSTRING_SIZE);
  memcpy(rt.identificator, hashOfKey, SHA256_OUTPUTSTRING_SIZE);
  rt.identificator[SHA256_OUTPUTSTRING_SIZE - 1] = 0;

  if (mirrors_count > 0) {
    rt.host_mirrors_count = mirrors_count;
    rt.host_mirrors = malloc(sizeof(char *) * mirrors_count + 1);
    for (unsigned int i = mirrors_count - 1; i--;) {
      size_t ms = strlen(mirrors[i]);
      rt.host_mirrors[i] = malloc(sizeof(char) * ms);
      memcpy(rt.host_mirrors[i], mirrors[i], ms);
    }
    rt.host_mirrors[mirrors_count] = 0;
  }
  return rt;
}

char **split_msg(char *buf, const char schar, size_t *splitted_size,
                 size_t msg_len) {
  /*
   */
  const unsigned long long max_splitted = 120;
  size_t arr_size = 0;
  char **splitted;
  splitted = (char **)malloc(sizeof(char *) * arr_size);
  if (!splitted)
    abort();
  char *str;
  char str2[msg_len];
  do {
    bzero(str2, msg_len);
    str = strchr(buf, schar);

    if (str != NULL || (str = strchr(buf, '\r')) != NULL) {
      arr_size++;
      splitted = (char **)realloc(splitted, sizeof(char *) * arr_size);
      if (!splitted)
        abort();

      memcpy(str2, buf, (str - buf));
      // printf("str2 = %s\n", str2);

      splitted[arr_size - 1] = (char *)malloc(sizeof(char) * strlen(str2) + 1);
      if (!splitted[arr_size - 1])
        abort();

      strcpy(splitted[arr_size - 1], str2);
      splitted[arr_size - 1][strlen(str2)] = 0;
    }
    buf = str + 1;
  } while (str != NULL && arr_size < max_splitted);
  *splitted_size = arr_size;

  return splitted;
}

void free_splitted(char **what, size_t n) {
  for (n; n--;) {
    if (n == 0)
      break;
    if (what[n][0] == 0)
      continue;
    free((void *)(what[n - 1]));
  }
  free((void *)what);
}


// fsplitter = '='; ssplitter=';' will be by default
char ** unpackData(char * data, size_t * rt_size, char fsplitter, char ssplitter){

}

char * join_data(const char * a, const char *b, const char split_char){
	size_t sA = strlen(a);
	size_t sB = strlen(b);
//	size_t lSize = 0;
	char * ret = malloc(sizeof(char) * (sA+sB)+3); // ';' + ';' + \0
	memcpy(ret, a, sA);
//	lSize += sA;
	ret[sA]=split_char;
//	lSize++;
	memcpy((ret+sA+1), b, sB); // plus ';'
//	lSize+=sB;
	ret[sA+sB+1] = split_char;
//	lSize++; 
	ret[sA+sB+2] = '\0'; //plus ';' + ';'
	return ret;
}

char * join_addresses(const char * addr, ...){

	va_list ap;
	va_start(ap, addr);
	size_t cSize = strlen(addr);
	char * pRet = malloc(cSize+1*sizeof(char));//sizeof of char is 1 on must of OS
	//void * fRet = pRet;
	//bzero(pRet, cSize);
	strcpy(pRet, addr);
	pRet[cSize] = SPLITADDRCHAR;

	addr = va_arg(ap, char*);

	while( addr != NULL ){
		size_t addr_size =strlen(addr)+1;
		pRet = (char*)realloc( pRet, ( addr_size+ cSize + 1) * sizeof(char) );
		memcpy(pRet+cSize+1, addr, addr_size);
		cSize+=addr_size;
		pRet[cSize] = SPLITADDRCHAR;
		addr = va_arg(ap, char*);

	}

	pRet = realloc(pRet,cSize + 1);
	pRet[cSize] = '\0';
	return	pRet;

}
/*
struct opcode{
	char opcode[OPCODELEN];
	opcodefun fun;
	peertype allowpeertypes;
	bool need_encryption;
};
*/
static const struct opcode InitOpcode =

	{
	        {0x01,0x02,0x03,0x04},
		NULL,
		ALLTYPESPERR,
		false
	};



void init_talk(struct peer * p, bool isUDP){
	
	set_timeout(p->sock_tcp,12, 0);
 	set_timeout( *(p->sock_udp),12, 0);
	char buf[NETDATASIZE];
	size_t ret_size;
	if(!isUDP){
		ret_size = recv(p->sock_tcp,  buf, NETDATASIZE, O_NONBLOCK);
	}else{
		ret_size = recv( *(p->sock_udp),  buf, NETDATASIZE, O_NONBLOCK);
	}
	if(ret_size <= 0) return;
	buf[ret_size]=0;
	//
}

#define INITTALKPRE(rt,T)\
	rt.sock_tcp = sock_tcp;\
	rt.sock_udp = sock_udp;\
	rt.addr_in=addr;\
	init_talk(&rt, T);\
	if(rt.identificator == NULL){ free_peer(&rt); close(rt.sock_tcp);close( *(rt.sock_udp) );rt.sock_tcp=0;rt.sock_udp=0; }
	
struct peer connect_to_peer(char *host, uint16_t port, int *sock_udp) {
  struct peer rt;
  if (sock_udp == NULL || *sock_udp == 0) {
    *sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
  }
  int sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_tcp <= 0 || *sock_udp <= 0) {
    fprintf(stderr, "Can't init sockets!!!\n");
    return rt;
  }
  struct sockaddr_in addr;
  socklen_t addr_l = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(host);

  int ret = connect(*sock_udp, (struct sockaddr *)&addr, addr_l);
  if (ret >= 0)
    rt.allow_con = (rt.allow_con | CON_UDP);
  else {
    // fprintf("INIT_PEER: %s:%d UNALLOW UDP\n");
    close(*sock_udp);
  }
  ret = connect(sock_tcp, (struct sockaddr *)&addr, addr_l);
  if (ret >= 0)
    rt.allow_con = (rt.allow_con | CON_TCP);
  else {
    close(sock_tcp);
  }
  if (rt.allow_con == CON_UNC)
    return rt;
  // connected trying to get keys and set identificator by the key.
 //TODO: exchange peers. timeout. 
 set_timeout(sock_tcp,1, 0);
 set_timeout(*sock_udp,1, 0);
 char buf[OPCODELEN];
 size_t ret_size = recv(sock_tcp,  buf, OPCODELEN, O_NONBLOCK);
{
 size_t sh = strlen(host);
 rt.host = malloc(sizeof(char)*sh+1);
 strcpy(rt.host, host);
 rt.host[sh] = 0;
}
 if(ret_size == OPCODELEN && strncmp(buf, InitOpcode.opcode, OPCODELEN) == 0){ // SUC TCP
	INITTALKPRE(rt, false);
	return rt;
 }else{
	ret_size = recv(*sock_udp, buf, OPCODELEN, O_NONBLOCK);
	if(ret_size == OPCODELEN && strncmp(buf, InitOpcode.opcode, OPCODELEN) == 0){
		INITTALKPRE(rt, true);
		return rt;
        } //suc UDP
	else{
		close(sock_tcp);
		close(*sock_udp);
		return rt;
	}// all bad
 }
 
}
