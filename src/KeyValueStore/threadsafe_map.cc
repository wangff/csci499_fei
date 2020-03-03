#include "threadsafe_map.h"

#include <mutex>
#include <unordered_map>

namespace cs499_fei {
bool ThreadsafeMap::Put(const std::string &key, const std::string &value) {
  std::lock_guard<std::mutex> lock(data_locker_);
  data_[key] = value;
  return true;
}

std::optional<std::string> ThreadsafeMap::Get(const std::string &key) const {
  std::lock_guard<std::mutex> lock(data_locker_);
  // key does not exist.
  if (!data_.count(key)) {
    return std::nullopt;
  }

  // return the value based on a key, if it exists in the hashmap.
  return data_.at(key);
}

void ThreadsafeMap::Remove(const std::string &key) {
  std::lock_guard<std::mutex> lock(data_locker_);
  data_.erase(key);
}
}  // namespace cs499_fei