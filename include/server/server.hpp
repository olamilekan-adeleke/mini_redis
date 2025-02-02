#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "../network/listening_socket.hpp"
#include <functional>
#include <stdio.h>
#include <unistd.h>

using RequestHandler =
    std::function<std::vector<uint8_t>(const uint8_t *, size_t)>;

class Server {
private:
  char buffer[1024] = {0};
  ListeningSocket *socket;
  RequestHandler handler;

  void handle_client(int client_socket);

public:
  Server();
  ~Server() { delete socket; }

  void set_handler(RequestHandler h);
  void start_server();
};

#endif
