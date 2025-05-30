#ifndef BINDING_SOCKET_H
#define BINDING_SOCKET_H

#include <netinet/in.h>

#include "socket.hpp"

class BindingSocket : public Socket {
 public:
  BindingSocket(int domain, int service, int port, u_long interface);

  // Virtual functions from parent class
  int connect_to_network(int sock, struct sockaddr_in address);
};

#endif  // BINDING_SOCKET_HPP
