#include "../../include/data_store/memory_data_store.hpp"

#include <iostream>
#include <optional>

MemoryDataStore::MemoryDataStore() {};

bool MemoryDataStore::set(const std::string *key, const std::string *val) {
  if (!key || !val) return false;

  try {
    std::string keyCopy(*key);
    std::string valCopy(*val);

    std::cout << "Key: '" << keyCopy << "'\n";
    std::cout << "Value: '" << valCopy << "'\n";

    store.try_emplace(std::move(keyCopy), std::move(valCopy));

    return true;
  } catch (const std::exception &e) {
    std::cerr << "Exception in set: " << e.what() << std::endl;
    return false;
  }
}

std::optional<std::string> MemoryDataStore::get(const std::string *key) const {
  if (store.count(*key)) {
    return store.at(*key);
  }

  return std::nullopt;
}
