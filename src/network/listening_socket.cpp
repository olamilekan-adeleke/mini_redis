#include "../../include/network/listening_socket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int port,
                                 u_long interface, int bklg)
    : BindingSocket(domain, service, port, interface) {
  this->backlog = bklg;
  start_listening();
  check_connection(listening);
}

void ListeningSocket::start_listening() {
  listening = listen(get_sock(), backlog);
}
