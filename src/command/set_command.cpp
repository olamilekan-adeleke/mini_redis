#include "../../include/command/set_command.hpp"
#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

SetCommand::SetCommand(string k, string val, optional<int32_t> ex,
                       optional<bool> nx)
    : BaseCommand("set"), key(std::move(k)), value(std::move(val)),
      exSeconds(ex), nx(nx) {}

SetCommand SetCommand::parseSetCommand(std::vector<std::string> parts) {
  if (parts.size() < 3) {
    // TODO: add more error details
    throw std::runtime_error("Invalid SET command");
  }

  std::string key = parts[1];
  std::string value = parts[2];
  optional<int32_t> exSeconds = std::nullopt;
  optional<bool> nx = std::nullopt;

  // Check the remainin items in a parts for the other optional params
  for (auto i = 3; i < parts.size(); i++) {
    std::string param = parts[i];
    std::transform(param.begin(), param.end(), param.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    if (param == "EX") {
      // TODO: added error check for int conversion
      exSeconds = std::stoi(parts[i + 1]);
    } else if (param == "NX") {
      nx = true;
    }
  }

  return SetCommand(key, value, exSeconds, nx);
}

std::vector<uint8_t> SetCommand::execute() {
  // save to memory and disk later
  //
  std::string res = "+OK\r\n";
  return std::vector<uint8_t>(res.begin(), res.end());
}
