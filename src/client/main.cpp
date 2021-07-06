#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>
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
#include <map>
//#include <gtk/gtk.h>

static int cPid = 0;

void stop_program(const char *msg, ...) {
  va_list l;
  va_start(l, msg);
  vfprintf(stderr, msg, l);
  va_end(l);
  if (cPid != 0)
    kill(cPid, SIGKILL);
  exit(1);
}

enum con_type { unconnected = 0, tcp = 1 << 1, udp = 1 << 2 };

using peer_pair_t = std::pair<std::string, uint16_t>;

struct sock_data {
  using msg_pair_t = std::pair<std::string, sock_data>;
  con_type m_contype;
  struct sockaddr_in m_adr;
  socklen_t m_adr_l;
  int *m_sock_tcp, *m_sock_udp;

  peer_pair_t m_peer_info;

  sock_data(struct sockaddr_in adr, socklen_t adr_l, int *sock_tcp,
            int *sock_udp, peer_pair_t peer_info, con_type type)
      : m_adr(adr), m_adr_l(adr_l), m_sock_udp(sock_udp), m_sock_tcp(sock_tcp),
        m_peer_info(peer_info), m_contype(type) {}

  sock_data(int *sock_tcp, int *sock_udp, peer_pair_t peer_info, con_type type)
      : m_sock_udp(sock_udp), m_sock_tcp(sock_tcp), m_peer_info(peer_info),
        m_contype(type) {
    m_adr.sin_family = AF_INET;
    m_adr.sin_port = htons(peer_info.second);
    m_adr.sin_addr.s_addr = inet_addr(peer_info.first.c_str());
    m_adr_l = sizeof(m_adr);
  }
  sock_data(void) {}
  void set_timeout(unsigned int tSec, unsigned int tUsec, bool isTCP = false) {
    int sock = isTCP ? *m_sock_tcp : *m_sock_udp;
    struct timeval tv;
    tv.tv_sec = tSec;
    tv.tv_usec = tUsec;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
  }
  int write(std::string data, bool isTCP = false) {
    int tmp = isTCP ? *m_sock_tcp : *m_sock_udp;
    if (isTCP && (m_contype & con_type::tcp != con_type::tcp))
      throw("TCP with peer unnallowed");
    if (!isTCP && (m_contype & con_type::udp != con_type::udp))
      throw("UDP with peer unnallowed");
    int c = sendto(tmp, data.c_str(), data.size(), 0, (struct sockaddr *)&m_adr,
                   m_adr_l);
    if (c < 0)
      throw(std::runtime_error(
          "Can't send to peer" +
          (m_peer_info.first + ":" + std::to_string(m_peer_info.second))));
    return c;
  }
  bool operator==(struct sockaddr_in &s) {
    return s.sin_family == m_adr.sin_family && s.sin_port == m_adr.sin_port &&
           m_adr.sin_addr.s_addr == s.sin_addr.s_addr;
  }

  msg_pair_t read(size_t sbuf = 1024, bool isTCP = false) {
    char buf[sbuf];
    bzero(buf, sbuf);
    int tmp = isTCP ? *m_sock_tcp : *m_sock_udp;

    struct sockaddr_in tadr = m_adr;
    socklen_t tadr_l = m_adr_l;

    int c =
        recvfrom(tmp, buf, sbuf, O_NONBLOCK, (struct sockaddr *)&tadr, &tadr_l);
    if (c < 0)
      throw(std::runtime_error(
          "Can't recv from peer " +
          (m_peer_info.first + ":" + std::to_string(m_peer_info.second))));
    sock_data n;
    if (!operator==(tadr)) {
      const char *ip = inet_ntoa(tadr.sin_addr);
      const uint16_t port = htons(tadr.sin_port);
      n = sock_data(m_sock_tcp, m_sock_udp, {ip, port}, con_type::udp);
    } else
      n = sock_data(m_sock_tcp, m_sock_udp, m_peer_info, con_type::udp);

    return {std::string(buf), n};
  }
};

using user_map = std::map<peer_pair_t, sock_data>;
const std::string conUserData = "PING!\n";

struct serv_data {
  using msg_pair_t = sock_data::msg_pair_t;

  con_type m_contype;
  int m_sock_udp;
  int m_sock_tcp;
  user_map users_map;
  sock_data m_data;

  struct sockaddr_in m_addr;

  void closeconn(void) {
    close(m_sock_tcp);
    close(m_sock_udp);
    // close(m_udp_self);
  }
  void self_bind(void) {
    int m_udp_self = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_udp_self <= 0)
      throw(std::runtime_error("Can't init sockets (SELF)"));

    write("getmypeerinfo\n");
    auto buf = read().first;
    puts("Buffer:");
    std::cout << buf << std::endl;

    std::string t = buf;
    std::string sIp = t.substr(1, t.find(':') - 1);
    std::string sPort =
        t.substr(t.find(':') + 1, t.find(']') - (t.find(':') + 1));
    std::cout << "Self IP: " << sIp << std::endl
              << "Self port: " << sPort << std::endl;

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(atoi(sPort.c_str()));
    m_addr.sin_addr.s_addr = inet_addr(sIp.c_str());

    close(m_sock_udp);

    int ret = bind(m_udp_self, (struct sockaddr *)&m_addr, sizeof(sockaddr_in));
    if (ret < 0) {
      stop_program("Can't bind(hole punch)\n");
    }
    m_sock_udp = m_udp_self;
  }
  void getUsers(void) {
    try {
      write("getuserlist\n");
      auto TODELETE = read().first;

      auto user_list_data = read().first;
      puts("users list:");
      auto addresses = split::split((char *)user_list_data.c_str(), ';',
                                    user_list_data.size());
      for (auto address : addresses) {
        std::cout << "Adr:" << address << std::endl;
        std::string ip = address.substr(0, address.find(':'));
        std::string port =
            address.substr(address.find(':') + 1, address.size());
        std::cout << "IP - " << ip << ", Port: " << port << std::endl;

        sock_data n_data = sock_data(&m_sock_tcp, &m_sock_udp,
                                     {ip, atoi(port.c_str())}, con_type::udp);

        n_data.write(conUserData);
        try {
          n_data.set_timeout(1, 0);
          auto r = n_data.read().first;
          if (r.size()) {
            users_map[{ip, atoi(port.c_str())}] = n_data;
            //			puts("New connection");
            //			sleep(35);//TODELETE
          }
        } catch (std::exception &e) {
          std::cout << e.what() << std::endl;
        }

      } // for
      // std::cout << "Read m_data" << std::endl;
      m_data.set_timeout(1, 0);

      auto r = m_data.read();
      // std::cout << "M_DATA" << r.first << std::endl;
      if (users_map.find(r.second.m_peer_info) == users_map.end()) {
        users_map[r.second.m_peer_info] = r.second;
        //		 puts("New connection");
        //		 sleep(35);//TODELETE
      }

      std::cout << "Users count: " << users_map.size() - 1 << std::endl;
      if (users_map.size() - 1 > 2) {
        std::cout << "Test passed!" << std::endl;
	m_data.write("abcd\n", true);
	auto b = m_data.read(1024,true);
	std::cout << "TCP RET:" << b.first << std::endl;
        sleep(1000);
      }
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
  }
  serv_data(std::string host, uint16_t port) : m_contype(unconnected) {
    m_sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    m_sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock_udp <= 0 || m_sock_tcp <= 0) {
      if (m_sock_udp > 0)
        close(m_sock_udp);
      if (m_sock_tcp > 0)
        close(m_sock_tcp);
      throw(std::runtime_error("Can't init sockets"));
    }
    struct sockaddr_in addr;
    socklen_t addr_l = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    int ret = connect(m_sock_udp, (struct sockaddr *)&addr, addr_l);
    if (ret >= 0)
      m_contype = (con_type)(m_contype | con_type::udp);
    else {
      std::cout << "Can't connect to UDP" << std::endl;
      close(m_sock_udp);
    }
    ret = connect(m_sock_tcp, (struct sockaddr *)&addr, addr_l);
    if (ret >= 0)
      m_contype = (con_type)(m_contype | con_type::tcp);
    else {
      std::cout << "Can't connect to TCP" << std::endl;
      close(m_sock_tcp);
    }
    if (m_contype == con_type::unconnected) {
      // closeconn();
      char errbuf[256];
      sprintf(errbuf, "Can't connect to %s:%d\n%c", host.c_str(), port, 0);
      throw(std::runtime_error(errbuf));
    } // connected
    m_data = sock_data(addr, addr_l, &m_sock_tcp, &m_sock_udp, {host, port},
                       m_contype);
  }
  int write(std::string data, bool isTCP = false) {
    return m_data.write(data, isTCP);
  }
  msg_pair_t read(size_t sbuf = 1024, bool isTCP = false) {
    return m_data.read(sbuf, isTCP);
  }

  int write(peer_pair_t peer_info, std::string data, bool isTCP = false) {
    return users_map[peer_info].write(data, isTCP);
  }
  msg_pair_t read(peer_pair_t peer_info, size_t sbuf = 1024,
                  bool isTCP = false) {
    return users_map[peer_info].read(sbuf, isTCP);
  }
};

int main(int argc, char **argv) {
  /*
  int pid = fork();
  if(pid != 0){
    cPid = pid;
    GtkWidget *window;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_show(window);
    gtk_main();
  }
  */

  if (argc != 3)
    stop_program("%s host port\n", argv[0]);
  serv_data sdata(argv[1], atoi(argv[2]));
  sdata.self_bind();
  while (1) {
    sdata.getUsers();
    sleep(1);
  }
}
