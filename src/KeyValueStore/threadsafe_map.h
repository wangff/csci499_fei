#ifndef CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_
#define CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "persistence_abstraction.h"
#include "persistence.h"
#include "../Func/func_platform.h"

namespace cs499_fei {
using PersistPtr = std::shared_ptr<PersistenceAbstraction>;
// Threadsafe hashmap which supports safe, concurrent access by multiple
// callers.
class ThreadsafeMap {
 public:
  // Default constructor
  ThreadsafeMap() : data_{}, data_locker_{} {};

  // Constructor with persistence
  ThreadsafeMap(const PersistPtr &persist_ptr, const std::string &file_name);

  // copy constructor
  ThreadsafeMap(const ThreadsafeMap&);

  // move constructor
  ThreadsafeMap &operator=(const ThreadsafeMap&);

  // Put a key-value pair to the store.
  bool Put(const std::string &key, const std::string &value);

  // Given the key, get the corresponding value from the store.
  // The return value std::optional<std::string>:
  // Either contains a string value, if the key exists in the map.
  // Or does not contain a value and is std::nullopt, if the key does not exist
  // int the map
  std::optional<std::string> Get(const std::string &key) const;

  // Given the key, remove the corresponding key-value pair from the store.
  void Remove(const std::string &key);

  // Store the in-memory data into the file
  void Store(const std::string &file_name);

 private:
  // A hashmap to save <key, value> pair.
  StringKVMap data_;

  // For thread safety, Use a mutex to avoid race condition.
  mutable std::mutex data_locker_;

  // Pointer to the persistence strategy
  PersistPtr persist_ptr_;

  // The local file to persist the data
  std::string file_name_;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_
