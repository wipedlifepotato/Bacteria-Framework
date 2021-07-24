#ifndef __clang__
module;
#endif
extern "C" {
#include "net.h"
}
#include <ctime>
#include <map>
//#include<chrono>

export module bnet;
/*
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
*/
export namespace bacteria {
  namespace net {
  enum mirrors_t { i2p, onion };
  enum peertype_t {
    client = 1 << 1,
    server = 1 << 2,
    bootstrap = 1 << 3,
    mediator = 1 << 4 // server<->mediator<->client
  };
  enum contype_t { unconnected = 1 << 0, UDP = 1 << 1, TCP = 1 << 2 };
  using map_mirror_t = std::map<mirrors_t, std::string>;
  class peer {
  private:
  protected:
    struct sockaddr_in addr_in;
    std::string host;
    uint16_t port;
    int sock_tcp, sock_udp;

  protected:
    std::time_t last_ping;

  protected:
    std::string shared_key;
    std::string ed25519;
    std::string x25519;
    std::string rsa;
    std::string identificator;

  protected:
    map_mirror_t mirrors;
    contype_t allowcon;
    peertype_t type;

  public:
  };
  } // namespace net

} // namespace bacteria
