#include "../../include/server/server.hpp"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

Server::Server() {
  int domain = AF_INET;
  int service = 0;
  int port = 6379;
  u_long interface = INADDR_ANY;
  int bklg = 10;

  socket = new ListeningSocket(domain, service, port, interface, bklg);
}

void Server::start_server() {
  while (true) {
    std::cout << "Server is Waiting for connection..." << std::endl;

    // Accept request
    struct sockaddr_in address = socket->get_address();
    auto sock_type = (socklen_t *)sizeof(address);
    const int new_socket =
        accept(socket->get_sock(), (struct sockaddr *)&address, sock_type);
    read(new_socket, buffer, 1024);

    // handle request
    std::cout << buffer << std::endl;

    // responder
    char *hello = "Hello Crack head";
    write(new_socket, hello, strlen(hello));

    std::cout << "Server is done responding to connection..." << std::endl;
  }
}
