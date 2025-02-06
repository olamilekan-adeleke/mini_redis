#include "../../include/parser/command_parser.hpp"
#include "../../include/command/command_error.hpp"
#include "../../include/command/set_command.hpp"
#include "../../include/parser/parser.hpp"
#include <iostream>
#include <memory>

CommandParser::CommandParser() {}

OptionalUniquePtr<BaseCommand>
CommandParser::parse(std::vector<uint8_t> &buffer) {
  RespParser resp_parser(buffer);

  try {

    std::any result = resp_parser.parse();

    if (result.type() == typeid(std::string)) {
      std::string str = std::any_cast<std::string>(result);
      // Handle Singel String later
      throw std::runtime_error("Unexpected route, got only STRING");

    } else if (result.type() == typeid(std::vector<std::any>)) {
      std::vector<std::string> array =
          std::any_cast<std::vector<std::string>>(result);

      if (array.size() == 0) {
        std::cout << "Empty array received." << std::endl;
        return std::make_unique<CommandError>("Empty array received.");
      } else if (array[0] == "SET") {

        return std::make_unique<SetCommand>(SetCommand::parseSetCommand(array));
      } else {
        std::cout << "Unimplemented command received." << std::endl;
        return std::make_unique<CommandError>("Unimplemented command received");
      }

    } else if (result.type() == typeid(std::nullptr_t)) {
      std::cout << "Null bulk string received." << std::endl;
      return std::make_unique<CommandError>("Null bulk string received");
    } else {
      std::cout << "Unknown type received." << std::endl;
      return std::make_unique<CommandError>("Unknown type received.");
    }
  } catch (const std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;

    // Todo: send out an error in Resdi protocol
    std::string err = "Error: " + std::string(e.what());
    return std::make_unique<CommandError>(err);
  }
}
