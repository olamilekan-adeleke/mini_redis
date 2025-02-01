#include "../include/network/binding_socket.hpp"
#include "../include/network/listening_socket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {

  std::cout << "Binding Scket..." << std::endl;
  BindingSocket bs = BindingSocket(AF_INET, SOCK_STREAM, 81, INADDR_ANY);

  std::cout << "Listening Socket..." << std::endl;
  ListeningSocket ls =
      ListeningSocket(AF_INET, SOCK_STREAM, 80, INADDR_ANY, SOMAXCONN);

  std::cout << "Done!" << std::endl;

  return 0;
}
