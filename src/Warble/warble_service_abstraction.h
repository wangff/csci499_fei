#ifndef CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_ABSTRACTION_H_
#define CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_ABSTRACTION_H_

#include <memory>
#include <optional>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <google/protobuf/any.pb.h>
#include <gtest/gtest_prod.h>

#include "../Func/storage_abstraction.h"
#include "profile.h"
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
using StringVector = std::vector<std::string>;
using StringOptional = std::optional<std::string>;
using StringOptionalVector = std::vector<StringOptional>;

// Abstraction class for WarbleService
// Create it for unit test as injection dependency
class WarbleServiceAbstraction {
 public:
  WarbleServiceAbstraction() = default;
  virtual ~WarbleServiceAbstraction() = default;

  // Register the given user_name
  virtual PayloadOptional RegisterUser(const Payload &payload, const StoragePtr &store) = 0;

  // Post a new warble or post a new warble as a reply,
  // and return the id of the new warble.
  virtual PayloadOptional WarbleText(const Payload &payload, const StoragePtr &store) = 0;

  // Start following a given user
  virtual PayloadOptional Follow(const Payload &payload, const StoragePtr &store) = 0;

  // Read a warble thread from the given id
  // Return the vector of the string serialization of Water protobuf
  virtual PayloadOptional ReadThread(const Payload &payload, const StoragePtr &store) = 0;

  // Return the given user's following and followers
  virtual PayloadOptional ReadProfile(const Payload &payload, const StoragePtr &store) = 0;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_ABSTRACTION_H_
