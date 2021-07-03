#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../bootstrap/utils.h"
#include <iostream>

#include <gtk/gtk.h>

int main(int argc, char **argv) {
  GtkWidget *window;
  gtk_init(&argc, &argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_show(window);
  gtk_main();

  if (argc != 3)
    return fprintf(stderr, "%s host port\n", argv[0]);
  int udp_sock_serv = socket(AF_INET, SOCK_DGRAM, 0);
  int udp_sock_self = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_sock_serv < 0 || udp_sock_self < 0)
    return fprintf(stderr, "Can't init sockets\n");

  struct sockaddr_in serv_addr;
  socklen_t socklen = sizeof(serv_addr);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[2]));
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  int ret = connect(udp_sock_serv, (struct sockaddr *)&serv_addr, socklen);
  if (ret < 0) {
    return fprintf(stderr, "Cant connect to %s:%d\n", argv[1], argv[2]);
  }
  char buf[1024];
  sprintf(buf, "initgetuserslist\n%c", '\0');
  sendto(udp_sock_serv, buf, strlen(buf), 0, (struct sockaddr *)&serv_addr,
         socklen);
  bzero(buf, sizeof(buf));
  recvfrom(udp_sock_serv, buf, sizeof(buf), 0, (struct sockaddr *)&serv_addr,
           &socklen);
  if (strlen(buf) == 0) {
    return fprintf(stderr, "null data from %s:%s\n", argv[1], argv[2]);
  }
  puts("Buffer:");
  printf("%s\n", buf);

  std::string t = buf;
  std::string sIp = t.substr(1, t.find(':') - 1);
  std::string sPort =
      t.substr(t.find(':') + 1, t.find(']') - (t.find(':') + 1));
  std::cout << "Self IP: " << sIp << std::endl
            << "Self port: " << sPort << std::endl;
  bzero(buf, sizeof(buf));
  recvfrom(udp_sock_serv, buf, sizeof(buf), 0, (struct sockaddr *)&serv_addr,
           &socklen);

  struct sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(atoi(sPort.c_str()));
  my_addr.sin_addr.s_addr = inet_addr(sIp.c_str());

  close(udp_sock_serv);

  ret = bind(udp_sock_self, (struct sockaddr *)&my_addr, sizeof(sockaddr_in));
  if (ret < 0) {
    return fprintf(stderr, "Can't bind(hole punch)\n");
  }
  puts("users list:");
  printf("%s\n", buf);
  auto addresses = split::split(buf, ';', sizeof(buf));

  const std::string p = "PING!\n";
// TODO:WITHOUT MACROS!
#define SENDTOALLHI                                                            \
  for (auto address : addresses) {                                             \
    std::cout << "Adr:" << address << std::endl;                               \
    std::string ip = address.substr(0, address.find(':'));                     \
    std::string port = address.substr(address.find(':') + 1, address.size());  \
    std::cout << "IP - " << ip << ", Port: " << port << std::endl;             \
                                                                               \
    struct sockaddr_in him_addr;                                               \
    him_addr.sin_family = AF_INET;                                             \
    him_addr.sin_port = htons(atoi(port.c_str()));                             \
    him_addr.sin_addr.s_addr = inet_addr(ip.c_str());                          \
    sendto(udp_sock_self, p.c_str(), p.size(), 0,                              \
           reinterpret_cast<struct sockaddr *>(&him_addr),                     \
           sizeof(struct sockaddr_in));                                        \
  }

  for (;;) {
    SENDTOALLHI;
    // puts("for;;");
    bzero(buf, sizeof(buf));
    // puts("recv");
    struct sockaddr_in hadr;
    socklen_t hadr_l;
    recvfrom(udp_sock_self, buf, sizeof(buf), 0, (struct sockaddr *)&hadr,
             &hadr_l);
    if (strlen(buf) > 0) {
      puts("Buffer:");
      const char *ip = inet_ntoa(hadr.sin_addr);
      const uint16_t port = htons(hadr.sin_port);
      printf("%s:%d %s\n", ip, port, buf);
      sendto(udp_sock_self, p.c_str(), p.size(), 0,
             reinterpret_cast<struct sockaddr *>(&hadr),
             sizeof(struct sockaddr_in));
    }
    // puts("usleep");
    usleep(500);
    // puts("endfor");
  }
  close(udp_sock_self);
}
