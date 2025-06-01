#ifndef SERVER_HPP
#define SERVER_HPP

#include <functional>
#include <memory>
#include <vector>

// Include Boost.Asio headers
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using RequestHandler = std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)>;

class Server {
 public:
  Server();
  ~Server();

  void set_handler(RequestHandler new_handler);
  void start_server();

 private:
  boost::asio::io_context io_context;
  boost::asio::ip::tcp::acceptor acceptor_;

  RequestHandler handler;

  void do_accept();
  void handle_client_fiber(std::shared_ptr<boost::asio::ip::tcp::socket> socket, boost::asio::yield_context yield);
};

#endif  // SERVER_HPP
