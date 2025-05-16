#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <unistd.h>

#include <boost/fiber/all.hpp>
#include <functional>

#include "../network/listening_socket.hpp"

using RequestHandler = std::function<std::vector<uint8_t>(const uint8_t *, size_t)>;

class Server {
 private:
  char buffer[1024];
  bool running;

  ListeningSocket *socket;
  RequestHandler handler;

  void handle_client(int client_socket);
  void accept_connections();

  // boost::fibers::condition_variable_any shutdown_cv;
  // boost::fibers::mutex shutdown_mutex;

 public:
  Server();
  ~Server() { delete socket; }

  void set_handler(RequestHandler h);
  void start_server();
  void stop_server();
};

#endif
