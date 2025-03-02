#ifndef LISTENING_SOCKET_H
#define LISTENING_SOCKET_H

#include "binding_socket.hpp"

class ListeningSocket : public BindingSocket {
 private:
  int backlog;
  int listening;

 public:
  ListeningSocket(int domain, int service, int port, u_long interface, int bklg);

  void start_listening();

  virtual ~ListeningSocket() = default;
};

#endif  // !LISTENING_SOCKET_H
