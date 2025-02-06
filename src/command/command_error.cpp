#include "../../include/command/command_error.hpp"

CommandError::CommandError(std::string err) : BaseCommand("ERROR") {
  this->err = std::move(err);
}

std::vector<uint8_t> CommandError::execute() {
  std::string resp = "-ERR " + err + "\r\n";
  return std::vector<uint8_t>(resp.begin(), resp.end());
}
