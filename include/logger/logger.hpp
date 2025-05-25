#include <fmt/format.h>

#include <string>

class Logger {
 public:
  template <typename S, typename... Args>
  static void log(const S& format_str, Args&&... args) {
    std::string formatted_message = fmt::format(format_str, std::forward<Args>(args)...);
    log_impl(formatted_message);
  }

  template <typename S, typename... Args>
  static void elog(const S& format_str, Args&&... args) {
    std::string formatted_message = fmt::format(format_str, std::forward<Args>(args)...);
    error_log_impl(formatted_message);
  }

  virtual ~Logger() = default;

 private:
  static void log_impl(const std::string& message);

  static void error_log_impl(const std::string& message);
};
