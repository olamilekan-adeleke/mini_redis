#include "../../include/data_store/memory_data_store.hpp"
#include <optional>

MemoryDataStore::MemoryDataStore() {};

bool MemoryDataStore::set(const std::string &key, const std::string &val) {
  store[key] = val;
  return true;
}

std::optional<std::string> MemoryDataStore::get(const std::string &key) const {
  if (store.count(key)) {
    return store.at(key);
  }

  return std::nullopt;
}
