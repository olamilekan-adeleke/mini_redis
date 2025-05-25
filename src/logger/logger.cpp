#include "../include/logger/logger.hpp"

#include <fmt/base.h>
#include <fmt/core.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

void Logger::log_impl(const std::string& message) {
  auto now = std::chrono::system_clock::now();
  std::time_t current_time = std::chrono::system_clock::to_time_t(now);
  std::tm* local_tm = std::gmtime(&current_time);

  std::cout << "[" << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S") << "] [LOG] " << message << std::endl;
}

void Logger::error_log_impl(const std::string& message) {
  auto now = std::chrono::system_clock::now();
  std::time_t current_time = std::chrono::system_clock::to_time_t(now);
  std::tm* local_tm = std::gmtime(&current_time);

  std::cout << "[" << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S") << "] [Error] " << message << std::endl;
}
