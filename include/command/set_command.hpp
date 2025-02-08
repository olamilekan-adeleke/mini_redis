#ifndef SET_COMMAND_H
#define SET_COMMAND_H

#include "../data_store/data_store.hpp"
#include "./command.hpp"
#include <vector>

using namespace std;

class SetCommand : public BaseCommand {
private:
  BaseDataStore &store;

public:
  SetCommand(BaseDataStore &store, string k, string val, optional<int32_t> ex,
             optional<bool> nx);

  std::string key;
  std::string value;
  std::optional<int32_t> exSeconds;
  std::optional<bool> nx;

  static SetCommand parseSetCommand(const std::vector<std::string> parts,
                                    BaseDataStore &store);

  virtual std::vector<uint8_t> execute() override;
};

#endif // !SET_COMMAND_H
