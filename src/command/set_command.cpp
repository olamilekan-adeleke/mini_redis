#include "../../include/command/set_command.hpp"

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

#include "../../include/data_store/data_store.hpp"

SetCommand::SetCommand(BaseDataStore &store, string k, string val, optional<int32_t> ex, optional<bool> nx)
    : BaseCommand("set"), store(store), key(std::move(k)), value(std::move(val)), exSeconds(ex), nx(nx) {}

SetCommand SetCommand::parseSetCommand(const std::vector<std::string> parts, BaseDataStore &store) {
  if (parts.size() < 3) {
    // TODO: add more error details
    throw std::runtime_error("Invalid SET command");
  }

  std::string key = parts[1];
  std::string value = parts[2];
  optional<int32_t> exSeconds = std::nullopt;
  optional<bool> nx = std::nullopt;

  // Check the remainin items in a parts for the other optional params
  for (int i = 3; i < static_cast<int>(parts.size()); i++) {
    std::string param = parts[i];
    std::transform(param.begin(), param.end(), param.begin(), [](unsigned char c) { return std::toupper(c); });

    if (param == "EX") {
      // TODO: added error check for int conversion
      exSeconds = std::stoi(parts[i + 1]);
    } else if (param == "NX") {
      nx = true;
    }
  }

  return SetCommand(store, std::move(key), std::move(value), exSeconds, nx);
}

std::vector<uint8_t> SetCommand::execute() {
  std::string res;

  bool result = store.set(&key, &value);
  if (result) {
    res = "+OK\r\n";
  } else {
    res = "-ERR Unable to set key\r\n";
  }

  // save to memory and disk later
  return std::vector<uint8_t>(res.begin(), res.end());
}
