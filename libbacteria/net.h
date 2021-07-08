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
typedef enum{
	CON_UNC = 1<<1,
	CON_UDP=1<<2, CON_TCP=1<<3
}contype;


typedef enum{
	PEER_USER = 1<<1,
	PEER_SERVER=1<<2, PEER_BOOTSTRAP=1<<3
}peertype;

struct peer{
	char * ip;
	uint16_t port;
	int sock;
	contype allow_con;
	peertype type;
	char * shared_key; // with another peer
	char * ed25519_key; // pub
	char * x25519_key; // pub
	char * rsa_key; // pub
	char * identificator; // sha256 of pubkey x25519
};

