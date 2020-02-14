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
  auto user_warbles_key  = user_warbles_prefix + user_prefix + user_name;
  auto user_followers_key = user_followers_prefix + user_prefix + user_name;
  auto user_followings_key = user_followings_prefix + user_prefix + user_name;
  StringVector keys_vector = {user_warbles_key};
  auto user_warbles = kv_store_->Get(keys_vector).at(0);
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
  auto user_followings_key = user_followings_prefix + user_prefix + user_name;
  auto to_follow_followers_key = user_followers_prefix + user_prefix + to_follow;
  StringVector key_vector;
  key_vector.push_back(user_followings_key);
  key_vector.push_back(to_follow_followers_key);

  auto value_vector = kv_store_->Get(key_vector);
  auto user_followings = value_vector.at(0);
  auto to_follow_followers = value_vector.at(1);

  std::string new_to_follow_followers = user_name;
  std::string new_user_followings = to_follow;

  if(user_followings != std::nullopt) {
    new_user_followings = user_followings.value() +"," + new_user_followings;
  }

  if(to_follow_followers != std::nullopt) {
    new_to_follow_followers = to_follow_followers.value() + "," + new_to_follow_followers;
  }

  kv_store_->Put(user_followings_key, new_user_followings);
  kv_store_->Put(to_follow_followers_key, new_to_follow_followers);
}

Profile Warble::ReadProfile(const std::string &user_name) {
  auto user_followings_key = user_followings_prefix + user_prefix + user_name;
  auto user_followers_key = user_followers_prefix + user_prefix + user_name;
  StringVector key_vector;
  key_vector.push_back(user_followings_key);
  key_vector.push_back(user_followers_key);

  auto value_vector = kv_store_->Get(key_vector);
  auto user_followings = value_vector.at(0);
  auto user_followers = value_vector.at(1);

  Profile profile;

  if(user_followings != std::nullopt) {
    profile.profile_followings = deserialize(user_followings.value(),',');
  }

  if(user_followers != std::nullopt) {
    profile.profile_followers = deserialize(user_followers.value(),',');
  }
  return profile;
}