#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "../include/logger/Logger.hpp"
#include "../include/parser/command_parser.hpp"
#include "../include/server/server.hpp"

int main() {
  Server server = Server();

  auto handler = [](const std::string &request_command_str) -> std::string {
    Logger::log("Processing received command string: {}",
                request_command_str.substr(0, std::min(50UL, request_command_str.length())));

    std::vector<uint8_t> request_data(request_command_str.begin(), request_command_str.end());

    std::string hex_data;
    for (uint8_t byte : request_data) {
      char hex_byte[4];
      snprintf(hex_byte, sizeof(hex_byte), "%02x ", byte);
      hex_data += hex_byte;
    }
    Logger::log("Handler received {} bytes (hex): {}", request_data.size(), hex_data);

    if (request_command_str.length() >= 5 && request_command_str.rfind("close", 0) == 0) {
      Logger::log("Received 'close' command, signaling server shutdown (consider alternative handling).");
      exit(0);  // This will terminate the entire process.
    }

    std::string response_str;
    try {
      Logger::log("Attempting to parse command...");
      CommandParser command_parser = CommandParser();
      // Assuming parse takes `std::vector<uint8_t>&`
      OptionalUniquePtr<BaseCommand> command = command_parser.parse(request_data);

      if (command.has_value()) {
        Logger::log("Command parsed successfully, executing...");
        std::unique_ptr<BaseCommand> cmd = std::move(*command);
        std::vector<uint8_t> response_vec = cmd->execute();
        response_str = std::string(response_vec.begin(), response_vec.end());
        Logger::log("Response: {}", response_str);
      } else {
        Logger::log("No command parsed, returning OK");
        response_str = "+OK\r\n";
      }
    } catch (const std::runtime_error &e) {
      Logger::elog("Error during parsing/execution: {}", e.what());
      response_str = "-ERR " + std::string(e.what()) + "\r\n";
    } catch (const std::exception &e) {
      Logger::elog("Unexpected error: {}", e.what());
      response_str = "-ERR Internal server error\r\n";
    }

    std::string response_hex;
    for (char byte : response_str) {
      char hex_byte[4];
      snprintf(hex_byte, sizeof(hex_byte), "%02x ", static_cast<uint8_t>(byte));
      response_hex += hex_byte;
    }
    Logger::log("Handler sending response ({} bytes, hex): {}", response_str.length(), response_hex);

    return response_str;
  };

  server.set_handler(std::move(handler));
  Logger::log("Starting Server...");

  server.start_server();  // This call will block until the server is stopped.
  Logger::elog("Exiting main function after server stopped.");
  return 0;
}
