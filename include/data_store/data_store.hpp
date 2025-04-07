#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <string>

class BaseDataStore {
 public:
  virtual ~BaseDataStore() = default;

  virtual bool set(const std::string *key, const std::string *val) = 0;
  virtual std::optional<std::string> get(const std::string *key) const = 0;
};

#endif  // !DATA_STORE_H
