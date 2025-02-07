#include "../../include/parser/command_parser.hpp"
#include "../../include/command/command_error.hpp"
#include "../../include/command/set_command.hpp"
#include "../../include/parser/parser.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <memory>

CommandParser::CommandParser() {}

OptionalUniquePtr<BaseCommand>
CommandParser::parse(std::vector<uint8_t> &buffer) {
  RespParser resp_parser(buffer);

  try {

    std::vector<std::string> result = resp_parser.parse();

    printf("Array size: %lu\n", result.size());

    if (result.empty()) {
      std::cout << "Empty array received." << std::endl;
      return std::make_unique<CommandError>("Empty array received.");
    }

    // Print the array elements (as strings if possible)
    std::cout << "Array: [";
    for (const std::string &element : result) {
      std::cout << element << ", ";
    }
    std::cout << "]" << std::endl;

    std::string command = result[0];
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "SET") {
      return std::make_unique<SetCommand>(SetCommand::parseSetCommand(result));
    } else {
      std::cout << "Unimplemented command received." << std::endl;
      return std::make_unique<CommandError>("Unimplemented command received");
    }

  } catch (const std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;

    // Todo: send out an error in Resdi protocol
    std::string err = "Error: " + std::string(e.what());
    return std::make_unique<CommandError>(err);
  }
}
