#include <iostream>
#include <thread>  // std::thread
#include <unordered_map>
#include <mutex>
#include "threadsafe_map.h"

bool threadsafe_map::put(std::string key, std::string value) {
  std::lock_guard<std::mutex> lock(m);
  data[key] = value;
  return true;
}

std::string threadsafe_map::get(std::string key) {
  std::lock_guard<std::mutex> lock(m);
  if (data.empty()) {
    return "";
  }

  // key does not exist.
  if (!data.count(key)) {
    return "";
  }

  // create a shared ptr.
  return data[key];
}

void threadsafe_map::remove(std::string key) {
  std::lock_guard<std::mutex> lock(m);
  data.erase(key);
}