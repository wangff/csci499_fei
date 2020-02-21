#ifndef CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_ABSTRACTION_H_
#define CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_ABSTRACTION_H_

#include <memory>
#include <string>

#include <gtest/gtest_prod.h>

#include "../Func/storage_abstraction.h"
#include "profile.h"

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
  virtual bool RegisterUser(const std::string &user_name) = 0;

  // Post a new warble or post a new warble as a reply,
  // and return the id of the new warble.
  virtual std::string WarbleText(const std::string &user_name, const std::string &text, const StringOptional &reply_to) = 0;

  //Start following a given user
  virtual void Follow(const std::string &user_name, const std::string &to_follow) = 0;

  // Read a warble thread from the given id
  // Return the vector of the string serialization of Water protobuf
  virtual StringVector ReadThread(const std::string &warble_id) = 0;

  // Return the given user's following and followers
  virtual Profile ReadProfile(const std::string &user_name) = 0;
};
#endif //CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_ABSTRACTION_H_
