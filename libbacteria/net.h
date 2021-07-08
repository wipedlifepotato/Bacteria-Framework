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


typedef enum{
	CON_UNC = 1<<0,
	CON_UDP=1<<1, CON_TCP=1<<2
}contype;


typedef enum{
	PEER_USER = 1<<1,
	PEER_SERVER=1<<2, PEER_BOOTSTRAP=1<<3
}peertype;

struct peer{
	struct sockaddr_in addr_in;
	char * host;
	uint16_t port;
	int sock_tcp;
	int sock_udp;
	contype allow_con;
	peertype type;
	char * shared_key; // with another peer
	char * ed25519_key; // pub
	char * x25519_key; // pub
	char * rsa_key; // pub
	char * identificator; // sha256 of pubkey x25519

	char * host_mirrors[]; // i2p / onion / yggdrasil / etc mirrors.
};
#ifdef __cplusplus
}
#endif
