#include "func_platform.h"

FuncPlatform::FuncPlatform(const StoragePtr &storage) : kv_store_(storage) { };

// Register the service to handle function when specific event occur.
void FuncPlatform::Hook(const EventType &event_type, const FunctionType &function_type) {
  auto key_str = "event_"+std::to_string(event_type);
  auto value_str = function_type;
  kv_store_->Put(key_str,value_str);
};

// Unregister event from service, return true if service is unregistered
void FuncPlatform::Unhook(const EventType &event_type) {
  auto key_str = "event_"+ std::to_string(event_type);
  kv_store_->Remove(key_str);
};

// Execute handler function based on event type
Payload FuncPlatform::Execute(const EventType &event_type, const Payload &payload) {
  // 1. call handler function based on eventype
  // 2. get reply payload from handler function
  // 3. return the reply payload
  std::vector<std::string> keys;
  auto key = "event_"+std::to_string(event_type);
  keys.push_back(key);
  auto values = kv_store_->Get(keys);
  auto function_str = values.at(0);
  //TODO
  //run the corresponding handler function in warble
  //get the replay payload, and reply it
  return payload;
}

