#include "warble_service.h"

#include <iostream>
#include <string>
#include <sstream>

// Helper function: split string by delimiter
StringVector deserialize(const std::string &s, char delim) {
  std::istringstream iss(s);
  std::string item;
  StringVector res;
  while(getline(iss,item,delim)) {
    res.push_back(item);
  }
  return res;
}

bool Warble::RegisterUser(const std::string &user_name) {
  // Initialize user profile
  std::string user_warbles_key  = kUserWarblesPrefix + kUserPrefix + user_name;
  std::string user_followers_key = kUserFollowersPrefix + kUserPrefix + user_name;
  std::string user_followings_key = kUserFollowingsPrefix + kUserPrefix + user_name;
  StringVector keys_vector = {user_warbles_key};
  StringOptional user_warbles = kv_store_->Get(keys_vector).at(0);
  bool is_user_exist = false;
  if (user_warbles != std::nullopt) {
    is_user_exist = true;
  }
  if (is_user_exist) {
    return false;
  }
  kv_store_->Put(user_warbles_key, "");
  kv_store_->Put(user_followers_key, "");
  kv_store_->Put(user_followings_key, "");
  return true;
}

void Warble::Follow(const std::string &user_name, const std::string &to_follow) {
  std::string user_followings_key = kUserFollowingsPrefix + kUserPrefix + user_name;
  std::string to_follow_followers_key = kUserFollowersPrefix + kUserPrefix + to_follow;
  StringVector key_vector;
  key_vector.push_back(user_followings_key);
  key_vector.push_back(to_follow_followers_key);

  StringOptionalVector value_vector = kv_store_->Get(key_vector);
  StringOptional user_followings = value_vector.at(0);
  StringOptional to_follow_followers = value_vector.at(1);

  std::string new_to_follow_followers = user_name;
  std::string new_user_followings = to_follow;

  if (user_followings != std::nullopt) {
    new_user_followings = user_followings.value() +"," + new_user_followings;
  }

  if (to_follow_followers != std::nullopt) {
    new_to_follow_followers = to_follow_followers.value() + "," + new_to_follow_followers;
  }

  kv_store_->Put(user_followings_key, new_user_followings);
  kv_store_->Put(to_follow_followers_key, new_to_follow_followers);
}

Profile Warble::ReadProfile(const std::string &user_name) {
  std::string user_followings_key = kUserFollowingsPrefix + kUserPrefix + user_name;
  std::string user_followers_key = kUserFollowersPrefix + kUserPrefix + user_name;
  StringVector key_vector;
  key_vector.push_back(user_followings_key);
  key_vector.push_back(user_followers_key);

  StringOptionalVector value_vector = kv_store_->Get(key_vector);
  StringOptional user_followings = value_vector.at(0);
  StringOptional user_followers = value_vector.at(1);

  Profile profile;

  if (user_followings != std::nullopt) {
    profile.profile_followings = deserialize(user_followings.value(),',');
  }

  if (user_followers != std::nullopt) {
    profile.profile_followers = deserialize(user_followers.value(),',');
  }
  return profile;
}

std::string Warble::WarbleText(const std::string &user_name, const std::string &text, const StringOptional &reply_to) {
  std::string current_warble_id = std::to_string(warble_id_);
  warble_id_++;
  // Create key vector
  // 0: warble list for user_name
  // 1: Optional. warble list for reply_to
  std::string user_warble_key = kUserWarblesPrefix + kUserPrefix + user_name;
  StringVector key_vector = {user_warble_key};
  std::string warble_thread_key;
  if (reply_to != std::nullopt) {
    warble_thread_key = kWarbleThreadPrefix + kWarblePrefix + reply_to.value();
    key_vector.push_back(warble_thread_key);
  }

  StringOptionalVector value_vector = kv_store_->Get(key_vector);

  std::string warble_key = kWarblePrefix + current_warble_id;

  StringOptional user_warbles = value_vector.at(0);
  std::string new_user_warbles = current_warble_id;
  if (user_warbles != std::nullopt) {
    new_user_warbles = user_warbles.value() +"," + new_user_warbles;
  }

  kv_store_->Put(warble_key,text);
  kv_store_->Put(user_warble_key, new_user_warbles);

  if (reply_to != std::nullopt) {
    StringOptional warble_thread = value_vector.at(1);
    std::string new_warble_thread = current_warble_id;
    if (warble_thread != std::nullopt) {
      new_warble_thread = warble_thread.value() + "," + new_warble_thread;
    }
    kv_store_->Put(warble_thread_key, new_warble_thread);
  }

  return current_warble_id;
}