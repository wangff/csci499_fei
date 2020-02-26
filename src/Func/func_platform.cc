#include "func_platform.h"

namespace cs499_fei {
FuncPlatform::FuncPlatform(const StoragePtr &storage, const WarblePtr &warble)
    : kv_store_(storage), warble_service_(warble), hook_dict_({}) {}

// Register the service to handle function when specific event occur.
void FuncPlatform::Hook(const EventType &event_type,
                        const FunctionName &function_type) {
  hook_dict_[event_type] = function_type;
};

// Unregister event from service, return true if service is unregistered
void FuncPlatform::Unhook(const EventType &event_type) {
  hook_dict_.erase(event_type);
};

// Execute handler function based on event type
PayloadOptional FuncPlatform::Execute(const EventType &event_type,
                                      const Payload &payload) {
  // There is no hooking config.
  if (!hook_dict_.count(event_type)) {
    return PayloadOptional();
  }

  std::string functionName = hook_dict_.at(event_type);

  // There is no such function in Warble.
  if (!kFunctionMap.count(functionName)) {
    return PayloadOptional();
  }

  FunctionType func = kFunctionMap.at(functionName);
  PayloadOptional reply_payload_opt = func(*warble_service_, payload);
  return reply_payload_opt;
}
}  // namespace cs499_fei