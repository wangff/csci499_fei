#ifndef CSCI499_FEI_SRC_KEYVALUESTORE_PERSISTENCE_H_
#define CSCI499_FEI_SRC_KEYVALUESTORE_PERSISTENCE_H_

#include "persistence_abstraction.h"

namespace cs499_fei {
// Persistence strategy:
// Serialize and deserialize in-memory key-value data to the local file just
// once.
class Persistence : public PersistenceAbstraction {
 public:
  // Write the in-memory key-value data into the file.
  void serialize(const StringKVMap &kv_data, const std::string &to_file) override;

  // Read the key-value data from the file to the memory.
  StringKVMap deserialize(const std::string &from_file) override;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_KEYVALUESTORE_PERSISTENCE_H_
