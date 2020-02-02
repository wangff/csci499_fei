#ifndef CSCI499_FEI_SRC_FUNC_STORAGE_ABSTRACTION_H_
#define CSCI499_FEI_SRC_FUNC_STORAGE_ABSTRACTION_H_

#include <optional>
#include <string>
#include <vector>

using StringVector = std::vector<std::string>;
using StringOptional = std::optional<std::string>;
using StringOptionalVector = std::vector<StringOptional>;

class StorageAbstraction {
 public:
  StorageAbstraction() = default;
  virtual ~StorageAbstraction() = default;

  // Put a key-value pair
  virtual void Put(const std::string &, const std::string &) = 0;

  // Get values based on keys
  virtual StringOptionalVector Get(const StringVector &) = 0;

  // Remove a value based on a key
  virtual void Remove(const std::string &) = 0;
};

#endif //KVSTORE_SRC_FUNC_STORAGE_ABSTRACTION_H_
