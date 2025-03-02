#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <memory>
#include <vector>

#include "../command/command.hpp"

template <typename T>
using OptionalUniquePtr = std::optional<std::unique_ptr<T>>;

class CommandParser {
 public:
  CommandParser();

  static OptionalUniquePtr<BaseCommand> parse(std::vector<uint8_t> &buffer);
};

#endif  // !COMMAND_PARSER_H
