#ifndef BINDING_SOCKET_H
#define BINDING_SOCKET_H

#include "socket.hpp"
#include <netinet/in.h>

class BindingSocket : public Socket {
public:
  BindingSocket(int domain, int service, int port, u_long interface);

  // Virtual functions from parent class
  int connect_to_network(int sock, struct sockaddr_in address);
};

// ListeningSocket
class ListeningSocket : public BindingSocket {
private:
  int backlog;
  int listening;

public:
  ListeningSocket(int domain, int service, int port, u_long interface,
                  int bklg);

  void start_listening();
};

#endif // BINDING_SOCKET_HPP
