#ifndef CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_
#define CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

class ThreadsafeMap {
 public:
  ThreadsafeMap() : data_{}, data_locker_{} {};

  // Put a key-value pair to the store.
  bool Put(const std::string &key, const std::string &value);

  // Given the key, get the corresponding value from the store.
  std::optional<std::string> Get(const std::string &key) const;

  // Given the key, remove the corresponding key-value pair from the store.
  void Remove(const std::string &key);

 private:
  // A hashmap to save <key, value> pair.
  std::unordered_map<std::string, std::string> data_;

  // For thread safety, Use a mutex to avoid race condition.
  mutable std::mutex data_locker_;
};

#endif //CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_
