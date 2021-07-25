#include "bnet.h"
namespace bacteria {
namespace net {
bool peer::operator==(struct sockaddr_in &s) {
  return s.sin_family == mIn.sin_family && s.sin_port == mIn.sin_port &&
         s.sin_addr.s_addr == mIn.sin_addr.s_addr;
}

peer &peer::operator()(peer &&p) {
  killConnect();
  mHost = inet_ntoa(p.mIn.sin_addr);
  mPort = htons(p.mIn.sin_port);
  mIn.sin_family = p.mIn.sin_family;
  mIn.sin_port = p.mIn.sin_port;
  mIn.sin_addr.s_addr = p.mIn.sin_addr.s_addr;
  sock_tcp = p.sock_tcp;
  sock_udp = p.sock_udp;
  return *this;
}
void peer::killConnect(void) {
  close(sock_tcp);
  close(sock_udp);
}

} // namespace net

} // namespace bacteria
