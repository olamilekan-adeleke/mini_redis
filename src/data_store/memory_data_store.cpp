#include "../../include/data_store/memory_data_store.hpp"

#include <optional>

#include "../../include/logger/logger.hpp"

MemoryDataStore::MemoryDataStore() {};

bool MemoryDataStore::set(const std::string &key, const std::string &val) {
  // std::lock_guard<std::mutex> lock(store_mutex);

  try {
    Logger::log("Key: {}, Value: {}", key, val);

    // store.try_emplace(key, val);
    return true;
  } catch (const std::exception &e) {
    Logger::elog("Exception in set: {}", e.what());
    return false;
  }
}

std::optional<std::string> MemoryDataStore::get(const std::string &key) const {
  auto it = store.find(key);
  if (it != store.end()) {
    return it->second;
  }

  return std::nullopt;
}
