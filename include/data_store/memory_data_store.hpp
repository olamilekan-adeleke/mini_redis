#ifndef MEMORY_DATA_STORE_H
#define MEMORY_DATA_STORE_H

#include <map>

#include "data_store.hpp"

class MemoryDataStore : public BaseDataStore {
 private:
  std::map<std::string, std::string> store;
  mutable std::mutex store_mutex;

 public:
  MemoryDataStore();

  bool set(const std::string &key, const std::string &val);  // Add more options
  std::optional<std::string> get(const std::string &key) const;

  // other methods (del, exists) later sha
};

#endif  // !MEMORY_DATA_STORE_H
