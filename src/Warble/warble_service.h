#ifndef CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_H_
#define CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_H_

#include "warble_service_abstraction.h"

// Warble application: a collection of handler functions to process users requests
class WarbleService : public WarbleServiceAbstraction {
 public:
  // Definition of all of keys' prefix
  const std::string kUserPrefix = "user_";
  const std::string kUserWarblesPrefix = "user_warbles_";
  const std::string kUserFollowersPrefix = "user_followers_";
  const std::string kUserFollowingsPrefix = "user_followings_";
  const std::string kWarblePrefix = "warble_";
  const std::string kWarbleThreadPrefix = "warble_thread_";

  // Constructor with the parameter of StoragePtr.
  // StoragePtr used by Warble to communicate with KeyValueStore.
  explicit WarbleService(const StoragePtr &storage_ptr) : kv_store_(storage_ptr) {};

  // Register the given user_name
  bool RegisterUser(const std::string &user_name);

  // Post a new warble or post a new warble as a reply,
  // and return the id of the new warble.
  std::string WarbleText(const std::string &user_name, const std::string &text, const StringOptional &reply_to);

  //Start following a given user
  void Follow(const std::string &user_name, const std::string &to_follow);

  // Read a warble thread from the given id.
  // Return the vector of the string serialization of Warble protobuf.
  StringVector ReadThread(const std::string &warble_id);

  // Return the given user's following and followers
  Profile ReadProfile(const std::string &user_name);

  // Make the private member accessible by test.
  FRIEND_TEST(WarbleTest, shouldReturnNewWarbleIdWhenWarbleATextReplyToAnotherWarbleWithouReplies);
  FRIEND_TEST(WarbleTest, shouldReturnNewWarbleIdWhenWarbleATextToReplyToAnotherWarbleWithReplies);

 private:
  // Pointer of storage abstraction.
  // Used to access the KeyValue storage.
  StoragePtr kv_store_;

  // Current warble id.
  // Whenever create a new warble, assign this id to this new warble.
  // Then increment by 1.
  int warble_id_ = 1;
};

#endif //CSCI499_FEI_SRC_WARBLE_WARBLE_SERVICE_H_