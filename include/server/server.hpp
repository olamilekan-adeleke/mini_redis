#ifndef SERVER_H
#define SERVER_H

#include "../network/listening_socket.hpp"
#include <stdio.h>
#include <unistd.h>

class Server {
private:
  char buffer[1024] = {0};
  ListeningSocket *socket;
  void handle_client(int client_socket);

public:
  Server();
  void start_server();
};

#endif
