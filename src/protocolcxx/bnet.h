#include "net.h"
#include "lua/luaserv.h"
#include <ctime>
#include <map>
#include <vector>
#include<thread>

namespace bacteria {
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
    struct sockaddr_in mIn;
    std::string mHost;
    uint16_t mPort;
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
  void killConnect(void);
  bool operator==(struct sockaddr_in &s);
  peer& operator()(peer && p);

  };//class peer


  } // namespace net

} // namespace bacteria
