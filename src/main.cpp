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

  auto handler = [](const uint8_t *data, size_t length) -> std::vector<uint8_t> {
    boost::fibers::promise<std::vector<uint8_t>> promise;
    boost::fibers::future<std::vector<uint8_t>> future = promise.get_future();

    boost::fibers::fiber([&]() {
      if (length >= 4 && memcmp(data, "close", 4) == 0) {
        exit(0);
      }

      std::cout << std::endl;
      std::vector<uint8_t> req_data = {data, data + length};
      std::vector<uint8_t> response;

      try {
        CommandParser command_parser = CommandParser();
        OptionalUniquePtr<BaseCommand> command = command_parser.parse(req_data);

        if (command.has_value()) {
          std::unique_ptr<BaseCommand> cmd = std::move(*command);
          response = cmd->execute();
          Logger::log("Response: {}", std::string(response.begin(), response.end()).c_str());
        } else {
          std::string ok = "+OK\r\n";
          response = std::vector<uint8_t>(ok.begin(), ok.end());
        }

      } catch (const std::runtime_error &e) {
        Logger::elog("Error: {}", e.what());

        // Return an error response
        std::string err = "-ERR " + std::string(e.what()) + "\r\n";
        response = std::vector<uint8_t>(err.begin(), err.end());
      }

      promise.set_value(response);
    }).detach();

    return future.get();
  };

  server.set_handler(std::move(handler));
  boost::fibers::use_scheduling_algorithm<boost::fibers::algo::shared_work>();

  Logger::log("Starting Server...");
  server.start_server();

  Logger::elog("Exiting...");
  return 0;
}
