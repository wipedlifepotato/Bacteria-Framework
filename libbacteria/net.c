#include "net.h"

/*
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

	char * host_mirrors[]; // i2p / onion / yggdrasil / etc mirrors.
};
*/
void set_timeout(int socket, unsigned int tSec, unsigned int tUsec, bool isTCP) {
    struct timeval tv;
    tv.tv_sec = tSec;
    tv.tv_usec = tUsec;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
}

struct peer init_self_peer(char * host, uint16_t port, 
	const char * rsa_key_file, const char * ed25519_key_file, const char * x25519_privkey){
	struct aKeyPair p_ed25519 = ed25519rsa_initPrivKey(ed25519_key_file, ed25519);
	struct aKeyPair p_rsa = ed25519rsa_initPrivKey(rsa_key_file, aRSA);
	struct x25519_keysPair p_x25519 = x25519_initKeyPairFromFile(x25519_privkey);
	//is can be stealed from RAM. is because is will be readed every time now and now.

}

struct peer connect_to_peer(char * host, uint16_t port, int * sock_udp){
    struct peer rt;
    if(sock_udp == NULL || *sock_udp == 0){
		*sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    }
    int sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_tcp <= 0 || *sock_udp <= 0){
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
      //fprintf("INIT_PEER: %s:%d UNALLOW UDP\n"); 
      close(*sock_udp);
    }
    ret = connect(sock_tcp, (struct sockaddr *)&addr, addr_l);
    if (ret >= 0)
     rt.allow_con = (rt.allow_con | CON_TCP);
    else {
      close(sock_tcp);
    }
    if (rt.allow_con == CON_UNC) return rt;
    //connected trying to get keys and identificator   
}
