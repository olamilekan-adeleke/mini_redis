#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class Socket {
private:
  struct sockaddr_in address;
  int sock;
  int connection;

public:
  Socket(int domain, int service, int port, u_long interface);

  virtual int connect_to_network(int sock, struct sockaddr_in address) = 0;

  void check_connection(int);

  struct sockaddr_in get_address();
  int get_sock();
  int get_connection();

  void set_connection(int domain);
};

#endif // SOCKET_HPP
