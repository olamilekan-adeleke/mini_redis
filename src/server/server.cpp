#include "../../include/server/server.hpp"

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/fiber/all.hpp>
#include <cstring>    // For memcmp
#include <iostream>   // Added for std::cerr debugging
#include <memory>     // For std::shared_ptr
#include <stdexcept>  // For std::runtime_error
#include <string>
#include <vector>

#include "../../include/logger/Logger.hpp"

// Custom scheduler that integrates Boost.Fiber with Boost.Asio
class asio_scheduler : public boost::fibers::algo::algorithm {
 public:
  asio_scheduler(boost::asio::io_context& io_ctx) : io_context_(io_ctx) {}

  void awakened(boost::fibers::context* ctx) noexcept override { ready_queue_.push_back(*ctx); }

  boost::fibers::context* pick_next() noexcept override {
    if (ready_queue_.empty()) {
      return nullptr;
    }
    boost::fibers::context& ctx = ready_queue_.front();
    ready_queue_.pop_front();
    return &ctx;
  }

  bool has_ready_fibers() const noexcept override { return !ready_queue_.empty(); }

  void suspend_until(std::chrono::steady_clock::time_point const& time_point) noexcept override {
    if (std::chrono::steady_clock::now() < time_point) {
      // Run io_context for a short time to handle I/O
      io_context_.run_for(time_point - std::chrono::steady_clock::now());
    }
  }

  void notify() noexcept override {
    // Wake up the scheduler
  }

 private:
  boost::asio::io_context& io_context_;
  boost::fibers::scheduler::ready_queue_type ready_queue_;
};

Server::Server() : io_context(), acceptor_(io_context) {
  int port = 6379;
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  Logger::log("Server Listening on port: {}", std::to_string(port));
}

Server::~Server() {
  // io_context.stop(); // Consider if explicit stop is needed for your shutdown logic
}

void Server::set_handler(RequestHandler new_handler) { handler = std::move(new_handler); }

void Server::start_server() {
  if (!handler) {
    throw std::runtime_error("No request handler set");
  }

  // Remove the Boost.Fiber scheduling algorithm setup
  // boost::fibers::use_scheduling_algorithm<boost::fibers::algo::shared_work>();

  boost::fibers::use_scheduling_algorithm<asio_scheduler>(std::ref(io_context));

  Logger::log("Server is starting asynchronous accept loop...");
  do_accept();

  Logger::log("Starting io_context.run()...");
  io_context.run();
  Logger::log("Server io_context.run() loop finished. Server shutting down.");
}

void Server::do_accept() {
  auto client_socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);

  Logger::log("do_accept: Created new socket ({}), waiting for connection...", fmt::ptr(client_socket.get()));

  acceptor_.async_accept(*client_socket, [this, client_socket](const boost::system::error_code& ec) {
    if (!ec) {
      Logger::log("async_accept callback: Successfully accepted new connection for socket ({}).",
                  fmt::ptr(client_socket.get()));
      std::string remote_addr_str = "unknown";
      try {
        remote_addr_str = client_socket->remote_endpoint().address().to_string();
        Logger::log("Server accepted new connection from {} on socket ({})", remote_addr_str,
                    fmt::ptr(client_socket.get()));
      } catch (const boost::system::system_error& ex) {
        Logger::elog("Error getting remote endpoint for socket ({}): {}", fmt::ptr(client_socket.get()), ex.what());
      }

      // --- FIXED MINIMAL TEST ---
      Logger::log("Attempting to spawn MINIMAL TEST LAMBDA COROUTINE for socket ({})...",
                  fmt::ptr(client_socket.get()));
      try {
        // Capture client_socket by value (copying the shared_ptr) for the lambda
        auto socket_for_test_lambda = client_socket;
        boost::asio::spawn(io_context,  // Use io_context directly instead of acceptor's executor
                           [socket_for_test_lambda](boost::asio::yield_context test_yield) {
                             // CRITICAL DEBUG STEP for the minimal lambda
                             std::cerr << "!!!!!!!!!! MINIMAL TEST LAMBDA COROUTINE ENTERED (Socket: "
                                       << fmt::ptr(socket_for_test_lambda.get()) << ") !!!!!!!!!! " << std::endl;

                             Logger::log("<<<<< MINIMAL TEST LAMBDA COROUTINE EXECUTING for socket ({}) >>>>>",
                                         fmt::ptr(socket_for_test_lambda.get()));

                             // Simulate some async work
                             boost::asio::steady_timer timer(socket_for_test_lambda->get_executor());
                             timer.expires_after(std::chrono::milliseconds(100));
                             boost::system::error_code timer_ec;
                             timer.async_wait(test_yield[timer_ec]);

                             Logger::log("MINIMAL TEST: Timer completed with error: {}", timer_ec.message());

                             // Close the socket
                             boost::system::error_code close_ec;
                             socket_for_test_lambda->close(close_ec);
                             if (close_ec) {
                               Logger::elog("MINIMAL TEST LAMBDA: Error closing socket ({}): {}",
                                            fmt::ptr(socket_for_test_lambda.get()), close_ec.message());
                             } else {
                               Logger::log("MINIMAL TEST LAMBDA: Socket ({}) closed.",
                                           fmt::ptr(socket_for_test_lambda.get()));
                             }
                             Logger::log("<<<<< MINIMAL TEST LAMBDA COROUTINE FINISHED for socket ({}) >>>>>",
                                         fmt::ptr(socket_for_test_lambda.get()));
                           });
        Logger::log("MINIMAL TEST LAMBDA COROUTINE spawn call completed for socket ({}).",
                    fmt::ptr(client_socket.get()));

      } catch (const std::exception& e_test) {
        Logger::elog("Failed to spawn MINIMAL TEST LAMBDA COROUTINE for socket ({}): {}", fmt::ptr(client_socket.get()),
                     e_test.what());
        // If even the test lambda fails to spawn, close the client socket
        boost::system::error_code close_ec;
        client_socket->close(close_ec);
      }
      // --- END OF MINIMAL COROUTINE TEST ---

    } else {
      Logger::elog("Accept failed: {} (code: {})", ec.message(), ec.value());
    }

    if (ec != boost::asio::error::operation_aborted && acceptor_.is_open()) {
      Logger::log("Calling do_accept again to continue listening...");
      do_accept();
    } else {
      Logger::log("Not calling do_accept again. Error: '{}' or acceptor closed.", ec.message());
    }
  });

  Logger::log("do_accept: async_accept call initiated, returning from function call...");
}

// Definition of handle_client_fiber (updated to use coroutines)
void Server::handle_client_fiber(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                                 boost::asio::yield_context yield) {
  std::cerr << "!!!!!!!!!! ORIGINAL HANDLE_CLIENT_COROUTINE ENTERED (Socket: " << fmt::ptr(socket.get())
            << ") !!!!!!!!!! " << std::endl;
  Logger::log("=== ORIGINAL COROUTINE STARTED for socket ({}) ===", fmt::ptr(socket.get()));

  std::string client_address_str = "unknown_client";
  try {
    if (socket && socket->is_open()) {
      boost::system::error_code rep_ec;
      boost::asio::ip::tcp::endpoint remote_ep = socket->remote_endpoint(rep_ec);
      if (!rep_ec) {
        client_address_str = remote_ep.address().to_string() + ":" + std::to_string(remote_ep.port());
        Logger::log("Coroutine handling client {} on socket ({})", client_address_str, fmt::ptr(socket.get()));
      } else {
        Logger::elog("Coroutine for socket ({}): Could not get remote endpoint: {}", fmt::ptr(socket.get()),
                     rep_ec.message());
        boost::system::error_code ignored_ec;
        socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        socket->close(ignored_ec);
        return;
      }
    } else {
      Logger::elog("Socket is null or not open in coroutine");
      return;
    }
  } catch (const std::exception& e) {
    Logger::elog("Exception in coroutine: {}", e.what());
    return;
  }

  // Do actual work here - for example, read from the socket
  try {
    std::array<char, 1024> buffer;
    boost::system::error_code read_ec;

    // Async read with coroutine
    std::size_t bytes_read = socket->async_read_some(boost::asio::buffer(buffer), yield[read_ec]);

    if (!read_ec && bytes_read > 0) {
      Logger::log("Read {} bytes from client {}", bytes_read, client_address_str);
      // Echo the data back
      boost::system::error_code write_ec;
      socket->async_write_some(boost::asio::buffer(buffer, bytes_read), yield[write_ec]);
      if (write_ec) {
        Logger::elog("Write error for {}: {}", client_address_str, write_ec.message());
      }
    } else if (read_ec) {
      Logger::log("Read error for {}: {}", client_address_str, read_ec.message());
    }
  } catch (const std::exception& e) {
    Logger::elog("Exception during socket operations for {}: {}", client_address_str, e.what());
  }

  // Clean shutdown
  Logger::log("Original handle_client_coroutine for {} is finishing up.", client_address_str);
  boost::system::error_code ignored_ec;
  socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  socket->close(ignored_ec);
  Logger::log("Original handle_client_coroutine for {} socket closed.", client_address_str);
}
