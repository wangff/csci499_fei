#ifndef CSCI499_FEI_SRC_KEYVALUESTORE_PERSISTENCE_ABSTRACTION_H_
#define CSCI499_FEI_SRC_KEYVALUESTORE_PERSISTENCE_ABSTRACTION_H_

#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace cs499_fei {
using StringKVMap = std::unordered_map<std::string, std::string>;

// The Abstraction for storage persistence strategy
class PersistenceAbstraction {
 public:
  PersistenceAbstraction() = default;
  virtual ~PersistenceAbstraction() = default;

  // Write the in-memory key-value data into the file.
  virtual void serialize(const StringKVMap &kv_data,
                         const std::string &to_file) = 0;

  // Read the key-value data from the file to the memory.
  virtual StringKVMap deserialize(const std::string &from_file) = 0;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_KEYVALUESTORE_PERSISTENCE_ABSTRACTION_H_
