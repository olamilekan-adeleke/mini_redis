// server.hpp
#pragma once

#include <functional>
#include <memory>
#include <string>

namespace oatpp {
namespace network {
class Server;
}
}  // namespace oatpp

class Server {
 public:
  using RequestHandler = std::function<std::string(const std::string&)>;

  Server();
  ~Server();

  void set_handler(RequestHandler new_handler);
  void start_server();
  void stop();

 private:
  RequestHandler handler;
  std::shared_ptr<oatpp::network::Server> server_;
};
