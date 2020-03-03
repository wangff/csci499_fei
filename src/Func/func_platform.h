#ifndef CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_
#define CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_

#include "keyvaluestore_client.h"

#include <sys/time.h>

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include <google/protobuf/any.pb.h>
#include <gtest/gtest_prod.h>

#include "../Warble/profile.h"
#include "../Warble/warble_service.h"
#include "Warble.grpc.pb.h"

using google::protobuf::Any;
using warble::FollowReply;
using warble::FollowRequest;
using warble::ProfileReply;
using warble::ProfileRequest;
using warble::ReadReply;
using warble::ReadRequest;
using warble::RegisteruserReply;
using warble::RegisteruserRequest;
using warble::Timestamp;
using warble::Warble;
using warble::WarbleReply;
using warble::WarbleRequest;

namespace cs499_fei {

using Payload = ::google::protobuf::Any;
using PayloadOptional = std::optional<Payload>;
using StoragePtr = std::shared_ptr<StorageAbstraction>;
using WarblePtr = std::shared_ptr<WarbleServiceAbstraction>;

using EventType = unsigned int;
using FunctionName = std::string;
using FunctionType =
    std::function<PayloadOptional(WarbleServiceAbstraction &, Payload)>;

using EventFuncNameMap = std::unordered_map<unsigned int, std::string>;
using StrFuncPair = std::unordered_map<std::string, FunctionType>;

const std::string kFunctionRegister = "register";
const std::string kFunctionWarble = "warble";
const std::string kFunctionFollow = "follow";
const std::string kFunctionRead = "read";
const std::string kFunctionProfile = "profile";

const StrFuncPair kFunctionMap = {
    {kFunctionRegister,
     std::function<PayloadOptional(WarbleServiceAbstraction &, Payload)>(
         &WarbleServiceAbstraction::RegisterUser)},
    {kFunctionWarble,
     std::function<PayloadOptional(WarbleServiceAbstraction &, Payload)>(
         &WarbleServiceAbstraction::WarbleText)},
    {kFunctionFollow,
     std::function<PayloadOptional(WarbleServiceAbstraction &, Payload)>(
         &WarbleServiceAbstraction::Follow)},
    {kFunctionRead,
     std::function<PayloadOptional(WarbleServiceAbstraction &, Payload)>(
         &WarbleServiceAbstraction::ReadThread)},
    {kFunctionProfile,
     std::function<PayloadOptional(WarbleServiceAbstraction &, Payload)>(
         &WarbleServiceAbstraction::ReadProfile)}};
// Faas platform support three features:
// 1. Event Management: Registration and removal if installed
// 2. Execute handler function in Warble.
// 3. Use storage abstraction to access KeyValue Store to do Put, Get, Remove
// operations.
class FuncPlatform {
 public:
  // Constructor with the injection of customized storage.
  FuncPlatform(const StoragePtr &, const WarblePtr &);

  // Register the service to handle function when specific event occur.
  void Hook(const EventType &, const FunctionName &);

  // Unregister event from service, return true if service is unregistered
  void Unhook(const EventType &);

  // Execute handler function based on event type
  PayloadOptional Execute(const EventType &, const Payload &);

  // Make private members could be accessed in unittest
  FRIEND_TEST(FuncPlatformTest, shouldHaveHookConfignAfterhookEventAndFunction);
  FRIEND_TEST(FuncPlatformTest, shouldNotHaveHookConfigAfterUnhookEvent);

 private:
  // Pointer of storage abstraction.
  // Used to access the KeyValue storage.
  StoragePtr kv_store_;

  // Pointer of Warble.
  // Used to access the handler functions.
  WarblePtr warble_service_;

  // A hash map to store hooking information of event type and the corresponding
  // function name.
  EventFuncNameMap hook_dict_;

  // For thread safety of hook_dict_, Use a mutex to avoid race condition.
  mutable std::mutex hook_dict_locker_;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_FUNC_FUNC_PLATFORM_H_
