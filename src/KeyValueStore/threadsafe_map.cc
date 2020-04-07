#include "threadsafe_map.h"

#include <mutex>
#include <unordered_map>

namespace cs499_fei {
// Constructor with persistence flag
ThreadsafeMap::ThreadsafeMap(const PersistPtr &persist_ptr, const std::string &file_name) {
  persist_ptr_ = persist_ptr;
  file_name_ = file_name;
  data_ = persist_ptr_->deserialize(file_name_);
}

// Copy constructor
ThreadsafeMap::ThreadsafeMap(const ThreadsafeMap &other) {
  std::lock_guard<std::mutex> lock(other.data_locker_);
  data_ = other.data_;
  file_name_ = other.file_name_;
  persist_ptr_ = other.persist_ptr_;
}

// Move constructor
ThreadsafeMap &ThreadsafeMap::operator=(const ThreadsafeMap &other) {
  if (&other != this) {
    std::lock_guard<std::mutex> lock(other.data_locker_);
    data_ = other.data_;
    file_name_ = other.file_name_;
    persist_ptr_ = other.persist_ptr_;
  }
  return *this;
}

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

void ThreadsafeMap::Store(const std::string &file_name) {
  if (!persist_ptr_) {
    return;
  }
  persist_ptr_->serialize(data_,file_name_);
}
}  // namespace cs499_fei