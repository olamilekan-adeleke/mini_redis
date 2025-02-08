#ifndef MEMORY_DATA_STORE_H
#define MEMORY_DATA_STORE_H

#include "data_store.hpp"
#include <unordered_map>

class MemoryDataStore : public BaseDataStore {
private:
  std::unordered_map<std::string, std::string> store;

public:
  MemoryDataStore();

  bool set(const std::string &key, const std::string &val);
  std::optional<std::string> get(const std::string &key) const;
};

#endif // !MEMORY_DATA_STORE_H
