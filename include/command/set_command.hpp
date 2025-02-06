#ifndef SET_COMMAND_H
#define SET_COMMAND_H

#include "./command.hpp"
#include <vector>

using namespace std;

class SetCommand : public BaseCommand {

public:
  SetCommand(string k, string val, optional<int32_t> ex, optional<bool> nx);

  std::string key;
  std::string value;
  std::optional<int32_t> exSeconds;
  std::optional<bool> nx;

  static SetCommand parseSetCommand(std::vector<std::string> parts);

  virtual std::vector<uint8_t> execute() override;

  ~SetCommand() override = default;
};

#endif // !SET_COMMAND_H
