#ifndef BINDING_SOCKET_H
#define BINDING_SOCKET_H

#include "socket.hpp"
#include <netinet/in.h>

class BindingSocket : public Socket {
public:
  BindingSocket(int domain, int service, int port, u_long interface);

  int connect_to_network(int sock, struct sockaddr_in address);
};

#endif // BINDING_SOCKET_HPP
