#include "../../include/network/socket.hpp"
#include <iostream>
#include <netinet/in.h>

// created using this C implementation
// https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

Socket::Socket(int domain, int service, int port, u_long interface) {

  // define address
  address.sin_family = domain;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(interface);

  // establish socket
  int protocol = 0;
  this->sock = socket(domain, service, protocol);
  this->check_connection(sock);

  // establish connection
  // this->connection = connect_to_network(sock, address);
  // this->check_connection(connection);
}

void Socket::check_connection(int item_to_check) {
  // check if sock or connection was established correctly
  if (item_to_check < 0) {
    perror("Failed to connect...");
    exit(EXIT_FAILURE);
  }
}

// Getters
struct sockaddr_in Socket::get_address() { return address; }

int Socket::get_sock() { return sock; }

int Socket::get_connection() { return connection; }

// Setters
void Socket::set_connection(int conn) { connection = conn; }
