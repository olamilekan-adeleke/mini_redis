#include "../../include/data_store/memory_data_store.hpp"

#include <optional>

#include "../../include/logger/logger.hpp"

MemoryDataStore::MemoryDataStore() {};

bool MemoryDataStore::set(const std::string *key, const std::string *val) {
  if (!key || !val) return false;

  try {
    std::string keyCopy(*key);
    std::string valCopy(*val);
    Logger::log("Key: {}, Value: {}", keyCopy, valCopy);

    store.try_emplace(std::move(keyCopy), std::move(valCopy));
    return true;
  } catch (const std::exception &e) {
    Logger::elog("Exception in set: {}", e.what());
    return false;
  }
}

std::optional<std::string> MemoryDataStore::get(const std::string *key) const {
  if (store.count(*key)) {
    return store.at(*key);
  }

  return std::nullopt;
}
