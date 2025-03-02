#ifndef CONNECTING_SOCKET_H
#define CONNECTING_SOCKET_H

#include <netinet/in.h>
#include <stdio.h>

#include "socket.hpp"

class ConnectingSocket : public Socket {
 public:
  ConnectingSocket(int domain, int service, int port, u_long interface);

  // Virtual functions from parent class
  int connect_to_network(int sock, struct sockaddr_in address);
};

#endif  // CONNECTING_SOCKET_H
