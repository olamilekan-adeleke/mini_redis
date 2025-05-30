#include "../../include/server/server.hpp"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server() {
  int domain = AF_INET;
  int service = SOCK_STREAM;
  int port = 6379;
  u_long interface = INADDR_ANY;
  int bklg = 10;

  std::cout << "Server Listening on port :" << port << " ..." << std::endl;
  socket = new ListeningSocket(domain, service, port, interface, bklg);
}

void Server::set_handler(RequestHandler new_handler) {
  handler = std::move(new_handler);
}

void Server::start_server() {
  if (!handler) {
    throw std::runtime_error("No request handler set");
  }

  while (true) {
    std::cout << "Server is Waiting for connection..." << std::endl;

    // Accept request
    struct sockaddr_in address = socket->get_address();
    socklen_t addrlen = sizeof(address);

    const int client_socket =
        accept(socket->get_sock(), (struct sockaddr *)&address, &addrlen);

    if (client_socket < 0) {
      std::cerr << "Accept failed" << std::endl;
      continue;
    }

    handle_client(client_socket);
  }
}

void Server::handle_client(int client_socket) {
  while (true) {
    memset(buffer, 0, sizeof(buffer)); // clear buffer if not empty

    // read client request
    size_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
      if (bytes_read == 0) {
        std::cout << "Client disconnected" << std::endl;
      } else {
        std::cerr << "Read error" << std::endl;
      }
      break;
    }

    std::vector<uint8_t> response =
        handler(reinterpret_cast<const uint8_t *>(buffer), bytes_read);

    // Send response
    if (write(client_socket, response.data(), response.size()) < 0) {
      std::cerr << "Write error" << std::endl;
      break;
    }
  }

  close(client_socket);
}
