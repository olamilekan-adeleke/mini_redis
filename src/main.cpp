#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "../include/logger/Logger.hpp"
#include "../include/parser/command_parser.hpp"
#include "../include/server/server.hpp"

using namespace std;

// SET session_id "abc123" EX 300 NX

int main() {
  Server server = Server();

  auto handler = [](const uint8_t *data, size_t length) -> std::vector<uint8_t> {
    if (length >= 4 && memcmp(data, "close", 4) == 0) {
      exit(0);
    }

    std::cout << std::endl;
    std::vector<uint8_t> req_data = {data, data + length};

    try {
      CommandParser command_parser = CommandParser();
      OptionalUniquePtr<BaseCommand> command = command_parser.parse(req_data);

      if (command.has_value()) {
        std::unique_ptr<BaseCommand> cmd = std::move(*command);
        std::vector<uint8_t> resp = cmd->execute();

        Logger::log("Response: {}", std::string(resp.begin(), resp.end()).c_str());
        return resp;
      }

    } catch (const std::runtime_error &e) {
      Logger::elog("Error: {}", e.what());
    }

    // Return custom response
    std::string ok = "+OK\r\n";
    return std::vector<uint8_t>(ok.begin(), ok.end());
  };
  server.set_handler(std::move(handler));

  Logger::log("Starting Server...");
  server.start_server();

  Logger::elog("Exiting...");
  return 0;
}
