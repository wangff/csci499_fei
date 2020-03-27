#ifndef CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_H_
#define CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_H_

#include "warble_service_abstraction.h"

namespace cs499_fei {
// Warble application: a collection of handler functions to process users
// requests
class WarbleService : public WarbleServiceAbstraction {
 public:
  // Definition of all of keys' prefix
  const std::string kUserPrefix = "user_";
  const std::string kUserWarblesPrefix = "user_warbles_";
  const std::string kUserFollowersPrefix = "user_followers_";
  const std::string kUserFollowingsPrefix = "user_followings_";
  const std::string kWarblePrefix = "warble_";
  const std::string kWarbleThreadPrefix = "warble_thread_";

  // The label to show the user's profile has been initialization.
  const std::string kInit = "INIT";

  // Constructor with the parameter of StoragePtr.
  explicit WarbleService(){};

  // Register the given user_name
  PayloadOptional RegisterUser(const Payload &payload,
                               const StoragePtr &kv_store);

  // Post a new warble or post a new warble as a reply,
  // and return the id of the new warble.
  PayloadOptional WarbleText(const Payload &payload,
                             const StoragePtr &kv_store);

  // Start following a given user
  PayloadOptional Follow(const Payload &payload, const StoragePtr &kv_store);

  // Read a warble thread from the given id.
  // Return the vector of the string serialization of Warble protobuf.
  PayloadOptional ReadThread(const Payload &payload,
                             const StoragePtr &kv_store);

  // Return the given user's following and followers
  PayloadOptional ReadProfile(const Payload &payload,
                              const StoragePtr &kv_store);

  // Allow unit tests access private members
  FRIEND_TEST(
      WarbleTest,
      shouldReturnNewWarbleWhenNewWarbleReplyToAnotherWarbleWithoutReplies);
  FRIEND_TEST(
      WarbleTest,
      shouldReturnNewWarbleWhenNewWarbleReplyToAnotherWarbleWithReplies);

 private:
  // Current warble id.
  // Whenever create a new warble, assign this id to this new warble.
  // Then increment by 1.
  int warble_id_ = 1;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_H_