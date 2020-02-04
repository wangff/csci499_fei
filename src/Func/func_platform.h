#ifndef CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_
#define CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_

#include <string>
#include <unordered_map>

#include <google/protobuf/any.pb.h>

#include "keyvaluestore_client.h"

using EventType = unsigned int;
using FunctionType = std::string;
using Payload = ::google::protobuf::Any;
using StoragePtr = std::shared_ptr<StorageAbstraction>;

// Faas platform that three features
// 1. Event Management: Registration and removal if installed
// 2. Execute handler function in Warble.
// 3. Use storage abstraction to access KeyValue Store to do Put, Get, Remove operations.
class FuncPlatform {
 public:
  // Constructor with the injection of customized storage.
  explicit FuncPlatform(const StoragePtr &);

  // Register the service to handle function when specific event occur.
  void Hook(const EventType &, const FunctionType &);

  // Unregister event from service, return true if service is unregistered
  void Unhook(const EventType &);

  // Execute handler function based on event type
  Payload Execute(const EventType &, const Payload &);

 private:
  StoragePtr kv_store_;
};

#endif //CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_
