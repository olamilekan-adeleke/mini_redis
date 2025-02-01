#include "../../include/network/binding_socket.hpp"
#include <sys/socket.h>

BindingSocket::BindingSocket(int domain, int service, int port,
                             u_long interface)
    : Socket(domain, service, port, interface) {
  int connection = connect_to_network(get_sock(), get_address());
  set_connection(connection);
  check_connection(connection);
}

int BindingSocket::connect_to_network(int sock, struct sockaddr_in address) {
  return bind(sock, (struct sockaddr *)&address, sizeof(address));
}

// ListeningSocket
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
