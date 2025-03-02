#ifndef MEMORY_DATA_STORE_H
#define MEMORY_DATA_STORE_H

#include <unordered_map>

#include "data_store.hpp"

class MemoryDataStore : public BaseDataStore {
 private:
  std::unordered_map<std::string, std::string> store;

 public:
  MemoryDataStore();

  bool set(const std::string &key, const std::string &val);  // Add more options
  std::optional<std::string> get(const std::string &key) const;

  // other methods (del, exists) later sha
};

#endif  // !MEMORY_DATA_STORE_H
