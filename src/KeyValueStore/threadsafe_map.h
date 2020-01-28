#ifndef CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_
#define CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_

#include <unordered_map>
#include <string>
#include <mutex>

class threadsafe_map {
 private:
  std::unordered_map<std::string, std::string> data;
  mutable std::mutex m;

 public:
  threadsafe_map() {};

  bool put(std::string key, std::string value);

  std::string get(std::string key);

  void remove(std::string key);
};

#endif //CSCI499_FEI_SRC_KEYVALUESTORE_THREADSAFE_MAP_H_
