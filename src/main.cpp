#include "../include/server/server.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
  Server server = Server();
  RequestHandler handler = [](const uint8_t *data,
                              size_t length) -> std::vector<uint8_t> {
    if (length >= 4 && memcmp(data, "close", 4) == 0) {
      exit(0);
    }

    // Process other commands...
    // You can inspect raw bytes here:
    for (size_t i = 0; i < length; i++) {
      // std::cout << std::hex << (int)data[i] << " ";
      std::cout << static_cast<char>(data[i]);
      // std::cout << (int)data[i] << " ";
    }
    std::cout << std::endl;

    // Return custom response
    std::string ok = "+OK\r\n";
    return std::vector<uint8_t>(ok.begin(), ok.end());
  };
  server.set_handler(std::move(handler));

  std::cout << "Starting Server..." << std::endl;
  server.start_server();

  std::cout << "Done." << std::endl;
  return 0;
}
