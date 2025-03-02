#ifndef COMMAND_ERROR_H
#define COMMAND_ERROR_H

#include <vector>

#include "./command.hpp"

class CommandError : public BaseCommand {
 public:
  std::string err;
  CommandError(std::string err);

  virtual std::vector<uint8_t> execute() override;

  ~CommandError() override = default;
};

#endif  // !COMMAND_ERROR_H
