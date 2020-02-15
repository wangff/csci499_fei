#include "func_platform.h"

FuncPlatform::FuncPlatform(const StoragePtr &storage, const WarblePtr &warble)
    : kv_store_(storage), warble_service_(warble), hook_dict_({}) { }

// Register the service to handle function when specific event occur.
void FuncPlatform::Hook(const EventType &event_type, const FunctionType &function_type) {
  hook_dict_[event_type] = function_type;
};

// Unregister event from service, return true if service is unregistered
void FuncPlatform::Unhook(const EventType &event_type) {
  hook_dict_.erase(event_type);
};

// Execute handler function based on event type
Payload FuncPlatform::Execute(const EventType &event_type, const Payload &payload) {
  // 1. call handler function based on eventype
  // 2. get reply payload from handler function
  // 3. return the reply payload
  auto function_str = hook_dict_.at(event_type);
  //TODO
  //run the corresponding handler function in warble
  //get the replay payload, and reply it
  return payload;
}


