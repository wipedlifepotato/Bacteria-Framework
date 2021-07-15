#ifdef __cplusplus
extern "C" {
#endif
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>          /* See NOTES */
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

#include"encdec/x25519.h"
#include"encdec/rsa_ed25519.h"
#include"encdec/base64.h"
#include"encdec/AES.h"
#include"encdec/hashes.h"
#include"macros.h"
#include"lua/lua.h"
#define RSAKEYSIZE 8192
#define RSAPRIMARYCOUNT 4

#define SPLITADDRCHAR ';'
#define SPLITINFOCHAR 0x1E
#define IGNOREINFOBYTE 0x03

#define OPCODELEN 4
#define NETDATASIZE 1024


#define doExit(...)                                                            \
  {                                                                            \
    eprintf(__VA_ARGS__);                                                      \
    exit(EXIT_FAILURE);                                                        \
  }

#define FREEKEYPAIR(pair)                                                      \
  if (pair.pkey != NULL) {                                                     \
    EVP_PKEY_free(pair.pkey);                                                  \
    free(pair.privKey);                                                        \
    free(pair.pubKey);                                                         \
  }

#define FREEISNOTNULL(what)                                                    \
  if (what != NULL)                                                            \
    free(what);

#define PUTPARAMS_(params)\
	va_list ap;\
	va_start(ap, params);

#define PUTPARAMS()\
	PUTSPARAMS_(params);


typedef enum{
	CON_UNC = 1<<0,
	CON_UDP=1<<1, CON_TCP=1<<2
}contype;


typedef enum{
	PEER_USER = 1<<1,
	PEER_SERVER=1<<2, PEER_BOOTSTRAP=1<<3,
	PEER_MEDIATOR=1<<4//server<->mediator<->client
}peertype;
#define ALLTYPESPERR PEER_USER|PEER_SERVER|PEER_BOOTSTRAP|PEER_MEDIATOR

struct triad_keys{
	struct aKeyPair ed25519;
	struct aKeyPair rsa;
	struct x25519_keysPair x25519;
};

struct peer{
       // bool isSelf; // it take 1 BYTE!
	time_t lastPing;
	struct sockaddr_in addr_in;
	char * host;
	uint16_t port;
	int sock_tcp;
	int *sock_udp;
	contype allow_con;
	peertype type;
	char * shared_key; // with another peer
	char * ed25519_key; // pub
	char * x25519_key; // pub
	char * rsa_key; // pub
	char * identificator; // sha256 of pubkey x25519
	size_t host_mirrors_count;
	char ** host_mirrors; // i2p / onion / yggdrasil / etc mirrors.
};

#define INIT_OPCODEFUN(name)\
	int opcode_##name(lua_State *L, const char params[], ...)

typedef void (*opcodefun)(lua_State * L, const char params[], ... );
//INIT_OPCODEFUN(inittalk);
void init_talk(struct peer * p, bool isUDP);


struct opcode{
	char opcode[OPCODELEN];
	opcodefun fun;
	peertype allowpeertypes;
	bool need_encryption;
	//bool isUDP;
};

//funcs
struct peer connect_to_peer(char *host, uint16_t port, int *sock_udp) ;
struct peer init_self_peer(char *host, uint16_t port, contype allow_con,
                           peertype type, size_t mirrors_count, char *mirrors[],
                           struct triad_keys *keys);
void free_peer(struct peer *p);
struct triad_keys generateSelfPeerKeys(const char *ed25519file,
                                       const char *rsafile,
                                       const char *x25519file);
void peer_freeKeys(struct triad_keys *keys);
struct triad_keys init_self_keys(const char *rsa_key_file,
                                 const char *ed25519_key_file,
                                 const char *x25519_privkey);
void set_timeout(int socket, unsigned int tSec, unsigned int tUsec);

void getparams(lua_State * L, const char params[], va_list ap);

char * join_addresses(const char * addr, ...);
char * join_data(const char * a, const char *b, const char split_char);


char ** 
split_msg(char * buf, const char schar, size_t * splitted_size, size_t msg_len);

void 
free_splitted(char  ** what, size_t n);

// fsplitter = '='; ssplitter=';' will be by default
char ** unpackData(char * data, size_t * rt_size, char fsplitter, char ssplitter);
#ifdef __cplusplus
}
#endif
