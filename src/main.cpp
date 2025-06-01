#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <boost/fiber/all.hpp>
#include <iostream>

#include "../include/logger/Logger.hpp"
#include "../include/parser/command_parser.hpp"
#include "../include/server/server.hpp"

using namespace std;

// SET session_id "abc123" EX 300 NX

int main() {
  Server server = Server();

  auto handler = [](const std::vector<uint8_t> &request_data) -> std::vector<uint8_t> {
    // Log raw bytes as hex for debugging
    std::string hex_data;
    for (uint8_t byte : request_data) {
      char hex_byte[4];
      snprintf(hex_byte, sizeof(hex_byte), "%02x ", byte);
      hex_data += hex_byte;
    }
    Logger::log("Received {} bytes (hex): {}", request_data.size(), hex_data);

    // Log as string (for printable characters)
    std::string str_data;
    for (uint8_t byte : request_data) {
      if (byte >= 32 && byte < 127) {
        str_data += static_cast<char>(byte);
      } else if (byte == '\r') {
        str_data += "\\r";
      } else if (byte == '\n') {
        str_data += "\\n";
      } else {
        str_data += "\\x" + std::to_string(byte);
      }
    }
    Logger::log("Received data (interpreted): {}", str_data);

    const uint8_t *data = request_data.data();
    size_t length = request_data.size();

    // Check for close command
    if (length >= 5 && memcmp(data, "close", 5) == 0) {
      exit(0);
    }

    std::vector<uint8_t> response;
    try {
      Logger::log("Attempting to parse command...");
      CommandParser command_parser = CommandParser();
      std::vector<uint8_t> req_data = request_data;
      OptionalUniquePtr<BaseCommand> command = command_parser.parse(req_data);

      if (command.has_value()) {
        Logger::log("Command parsed successfully, executing...");
        std::unique_ptr<BaseCommand> cmd = std::move(*command);
        response = cmd->execute();
        Logger::log("Response: {}", std::string(response.begin(), response.end()));
      } else {
        Logger::log("No command parsed, returning OK");
        std::string ok = "+OK\r\n";
        response = std::vector<uint8_t>(ok.begin(), ok.end());
      }
    } catch (const std::runtime_error &e) {
      Logger::elog("Error during parsing/execution: {}", e.what());
      // Return an error response
      std::string err = "-ERR " + std::string(e.what()) + "\r\n";
      response = std::vector<uint8_t>(err.begin(), err.end());
    } catch (const std::exception &e) {
      Logger::elog("Unexpected error: {}", e.what());
      std::string err = "-ERR Internal server error\r\n";
      response = std::vector<uint8_t>(err.begin(), err.end());
    }

    // Log response as hex too
    std::string response_hex;
    for (uint8_t byte : response) {
      char hex_byte[4];
      snprintf(hex_byte, sizeof(hex_byte), "%02x ", byte);
      response_hex += hex_byte;
    }
    Logger::log("Sending response ({} bytes, hex): {}", response.size(), response_hex);

    return response;
  };

  server.set_handler(std::move(handler));
  // boost::fibers::use_scheduling_algorithm<boost::fibers::algo::shared_work>();
  Logger::log("Starting Server...");
  server.start_server();
  Logger::elog("Exiting...");
  return 0;
}
