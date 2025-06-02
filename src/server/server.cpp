#include "../../include/server/server.hpp"

#include <oatpp/core/base/Countable.hpp>

#include "../../include/logger/Logger.hpp"
#include "oatpp/network/ConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

class RedisConnectionHandler : public oatpp::network::ConnectionHandler {
 private:
  using oatpp::network::ConnectionHandler::handleConnection;

 public:
  RedisConnectionHandler(Server::RequestHandler handler) : request_handler_(std::move(handler)) {}

  static oatpp::Object<RedisConnectionHandler> createShared(Server::RequestHandler handler) {
    return oatpp::Object<RedisConnectionHandler>(new RedisConnectionHandler(std::move(handler)));
  }

  void handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection) {
    Logger::log("New connection established from client");

    // Create a buffer for reading data
    oatpp::v_io_size BUFFER_SIZE = 4096;
    auto buffer = std::make_unique<v_char8[]>(BUFFER_SIZE);
    std::string accumulated_data;

    try {
      while (true) {
        // Read data from the connection
        auto result = connection->readSimple(buffer.get(), BUFFER_SIZE);

        if (result > 0) {
          Logger::log("Read {} bytes from client", result);

          // Append to accumulated data
          accumulated_data.append(reinterpret_cast<char*>(buffer.get()), result);

          // Process complete commands (assuming RESP protocol ends with \r\n)
          size_t cmd_end;
          while ((cmd_end = accumulated_data.find("\r\n")) != std::string::npos) {
            // Extract one complete command
            std::string command = accumulated_data.substr(0, cmd_end + 2);
            accumulated_data.erase(0, cmd_end + 2);

            Logger::log("Processing command: {}", command.substr(0, std::min(50UL, command.length())));

            // Call your existing request handler
            std::string response = request_handler_(command);

            // Send response back to client
            if (!response.empty()) {
              auto bytesWritten = connection->writeSimple(response.c_str(), response.length());
              if (bytesWritten != static_cast<oatpp::v_io_size>(response.length())) {
                Logger::elog("Failed to write complete response to client");
                break;
              }
              Logger::log("Sent {} bytes response to client", bytesWritten);
            }
          }
        } else if (result == 0) {
          Logger::log("Client disconnected (EOF)");
          break;
        } else {
          Logger::elog("Error reading from client connection");
          break;
        }
      }
    } catch (const std::exception& e) {
      Logger::elog("Exception in connection handler: {}", e.what());
    }

    Logger::log("Connection handler finished");
  }

 private:
  Server::RequestHandler request_handler_;
};

// Updated Server implementation using oat++
Server::Server() {
  Logger::log("Initializing oat++ environment");
  oatpp::base::Environment::init();

  int port = 6379;
  Logger::log("Server will listen on port: {}", port);
}

Server::~Server() {
  if (server_) {
    server_->stop();
  }
  Logger::log("Destroying oat++ environment");
  oatpp::base::Environment::destroy();
}

void Server::set_handler(RequestHandler new_handler) {
  handler = std::move(new_handler);
  Logger::log("Request handler set");
}

void Server::start_server() {
  if (!handler) {
    throw std::runtime_error("No request handler set");
  }

  Logger::log("Creating oat++ TCP connection provider on port 6379");

  auto connectionProvider =
      oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", 6379, oatpp::network::Address::IP_4});
  const oatpp::Object<RedisConnectionHandler> connectionHandler = RedisConnectionHandler::createShared(handler);
  server_ = oatpp::network::Server::createShared(connectionProvider, connectionHandler);

  Logger::log("Starting oat++ server...");
  Logger::log("Server is ready to accept connections on port 6379");

  // This will block until server is stopped
  server_->run();

  Logger::log("Server stopped");
}

void Server::stop() {
  if (server_) {
    Logger::log("Stopping server...");
    server_->stop();
  }
}
