#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

#include "../include/server/server.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
  Server server = Server();
  server.start_server();

  std::cout << "Done!" << std::endl;

  return 0;
}
