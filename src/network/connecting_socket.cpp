#include "../../include/network/connecting_socket.hpp"

ConnectingSocket::ConnectingSocket(int domain, int service, int port,
                                   u_long interface)
    : Socket(domain, service, port, interface) {

  int connection = connect_to_network(get_sock(), get_address());
  set_connection(connection);
  check_connection(connection);
}

int ConnectingSocket::connect_to_network(int sock, struct sockaddr_in address) {
  return connect(sock, (struct sockaddr *)&address, sizeof(address));
}
