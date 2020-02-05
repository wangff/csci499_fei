#ifndef CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_
#define CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_

#include <string>
#include <unordered_map>

#include <google/protobuf/any.pb.h>
#include <gtest/gtest_prod.h>

#include "keyvaluestore_client.h"
#include "../Warble/warble_service.h"

using EventType = unsigned int;
using FunctionType = std::string;

using Payload = ::google::protobuf::Any;
using StoragePtr = std::shared_ptr<StorageAbstraction>;
using WarblePtr = std::shared_ptr<Warble>;
using EventPairSet = std::unordered_map<EventType, FunctionType>;

// Faas platform support three features:
// 1. Event Management: Registration and removal if installed
// 2. Execute handler function in Warble.
// 3. Use storage abstraction to access KeyValue Store to do Put, Get, Remove operations.
class FuncPlatform {
 public:
  // Constructor with the injection of customized storage.
  FuncPlatform(const StoragePtr &, const WarblePtr &);

  // Register the service to handle function when specific event occur.
  void Hook(const EventType &, const FunctionType &);

  // Unregister event from service, return true if service is unregistered
  void Unhook(const EventType &);

  // Execute handler function based on event type
  Payload Execute(const EventType &, const Payload &);

 private:
  // Pointer of storage abstraction.
  // Used to access the KeyValue storage.
  StoragePtr kv_store_;

  // Pointer of Warble.
  // Used to access the handler functions.
  WarblePtr warble_service_;

  // A hash map to store hooking information of event type and string
  EventPairSet hook_dict_;

 public:
  // Make private members could be accessed in unittest
  FRIEND_TEST(FuncPlatformTest, CanHook);
  FRIEND_TEST(FuncPlatformTest, CanUnhook);
};

#endif //CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_
