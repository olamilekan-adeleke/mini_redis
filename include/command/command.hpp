#ifndef BASE_COMMAND_H
#define BASE_COMMAND_H

#include <string>

class BaseCommand {
 public:
  BaseCommand(std::string name) : name(std::move(name)) {}

  std::string name;

  virtual std::vector<uint8_t> execute() = 0;
  virtual ~BaseCommand() = default;
};

#endif  // !BASE_COMMAND_H
