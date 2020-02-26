#include "warble_service.h"

#include <sys/time.h>

#include <sstream>
#include <string>

namespace cs499_fei {
// Helper function: split string by delimiter
StringVector deserialize(const std::string &s, char delim) {
  std::istringstream iss(s);
  std::string item;
  StringVector res;
  while (getline(iss, item, delim)) {
    res.push_back(item);
  }
  return res;
}

PayloadOptional WarbleService::RegisterUser(const Payload &payload) {
  RegisteruserRequest request;
  payload.UnpackTo(&request);

  std::string user_name = request.username();

  // Initialize user profile
  std::string user_warbles_key = kUserWarblesPrefix + kUserPrefix + user_name;
  std::string user_followers_key =
      kUserFollowersPrefix + kUserPrefix + user_name;
  std::string user_followings_key =
      kUserFollowingsPrefix + kUserPrefix + user_name;
  StringVector keys_vector = {user_warbles_key};
  StringOptional user_warbles = kv_store_->Get(keys_vector).at(0);

  bool is_user_exist = user_warbles != std::nullopt;

  RegisteruserReply reply;
  Payload reply_payload;
  reply_payload.PackFrom(reply);

  if (is_user_exist) {
    return PayloadOptional();
  }

  kv_store_->Put(user_warbles_key, "");
  kv_store_->Put(user_followers_key, "");
  kv_store_->Put(user_followings_key, "");
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::Follow(const Payload &payload) {
  FollowRequest request;
  payload.UnpackTo(&request);

  std::string user_name = request.username();
  std::string to_follow = request.to_follow();

  std::string user_followings_key =
      kUserFollowingsPrefix + kUserPrefix + user_name;
  std::string to_follow_followers_key =
      kUserFollowersPrefix + kUserPrefix + to_follow;
  StringVector key_vector;
  key_vector.push_back(user_followings_key);
  key_vector.push_back(to_follow_followers_key);

  StringOptionalVector value_vector = kv_store_->Get(key_vector);
  StringOptional user_followings = value_vector.at(0);
  StringOptional to_follow_followers = value_vector.at(1);

  std::string new_to_follow_followers = user_name;
  std::string new_user_followings = to_follow;

  if (user_followings != std::nullopt) {
    new_user_followings = user_followings.value() + "," + new_user_followings;
  }

  if (to_follow_followers != std::nullopt) {
    new_to_follow_followers =
        to_follow_followers.value() + "," + new_to_follow_followers;
  }

  kv_store_->Put(user_followings_key, new_user_followings);
  kv_store_->Put(to_follow_followers_key, new_to_follow_followers);

  FollowReply reply;
  Payload reply_payload;
  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::ReadProfile(const Payload &payload) {
  ProfileRequest request;
  payload.UnpackTo(&request);

  std::string user_name = request.username();
  std::string user_followings_key =
      kUserFollowingsPrefix + kUserPrefix + user_name;
  std::string user_followers_key =
      kUserFollowersPrefix + kUserPrefix + user_name;
  StringVector key_vector;
  key_vector.push_back(user_followings_key);
  key_vector.push_back(user_followers_key);

  StringOptionalVector value_vector = kv_store_->Get(key_vector);
  StringOptional user_followings = value_vector.at(0);
  StringOptional user_followers = value_vector.at(1);

  Profile profile;

  if (user_followings != std::nullopt) {
    profile.profile_followings = deserialize(user_followings.value(), ',');
  }

  if (user_followers != std::nullopt) {
    profile.profile_followers = deserialize(user_followers.value(), ',');
  }

  ProfileReply reply;
  for (const auto &following : profile.profile_followings) {
    reply.add_following(following);
  }
  for (const auto &follower : profile.profile_followers) {
    reply.add_followers(follower);
  }

  Payload reply_payload;
  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::WarbleText(const Payload &payload) {
  timeval time;
  gettimeofday(&time, NULL);

  WarbleRequest request;

  payload.UnpackTo(&request);
  std::string user_name = request.username();
  std::string text = request.text();
  std::string reply_to = request.parent_id();

  std::string current_warble_id = std::to_string(warble_id_);
  warble_id_++;

  // Create key vector
  // 0: warble list for user_name
  // 1: Optional. warble list for reply_to
  std::string user_warble_key = kUserWarblesPrefix + kUserPrefix + user_name;
  StringVector key_vector = {user_warble_key};
  std::string warble_thread_key;
  if (reply_to != "") {
    warble_thread_key = kWarbleThreadPrefix + kWarblePrefix + reply_to;
    key_vector.push_back(warble_thread_key);
  }

  StringOptionalVector value_vector = kv_store_->Get(key_vector);

  std::string warble_key = kWarblePrefix + current_warble_id;

  StringOptional user_warbles = value_vector.at(0);
  std::string new_user_warbles = current_warble_id;
  if (user_warbles != std::nullopt) {
    new_user_warbles = user_warbles.value() + "," + new_user_warbles;
  }

  kv_store_->Put(warble_key, text);
  kv_store_->Put(user_warble_key, new_user_warbles);

  if (reply_to != "") {
    StringOptional warble_thread = value_vector.at(1);
    std::string new_warble_thread = current_warble_id;
    if (warble_thread != std::nullopt) {
      new_warble_thread = warble_thread.value() + "," + new_warble_thread;
    }
    kv_store_->Put(warble_thread_key, new_warble_thread);
  }

  WarbleReply reply;
  reply.mutable_warble()->set_username(user_name);
  reply.mutable_warble()->set_text(text);
  reply.mutable_warble()->set_id(current_warble_id);
  reply.mutable_warble()->set_parent_id(reply_to);
  reply.mutable_warble()->mutable_timestamp()->set_seconds(time.tv_sec);
  reply.mutable_warble()->mutable_timestamp()->set_useconds(time.tv_usec);

  Payload reply_payload;
  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::ReadThread(const Payload &payload) {
  ReadRequest request;
  payload.UnpackTo(&request);
  std::string warble_id = request.warble_id();

  StringVector warbles_str_vector;
  std::string warble_thread_key =
      kWarbleThreadPrefix + kWarblePrefix + warble_id;
  StringVector key_vector = {warble_thread_key};
  StringOptional warble_ids_opt = kv_store_->Get(key_vector).at(0);
  std::string warble_ids_str = "";
  if (warble_ids_opt != std::nullopt && warble_ids_opt.has_value()) {
    warble_ids_str = warble_ids_opt.value();
  }

  ReadReply reply;
  Payload reply_payload;

  if (warble_ids_str.empty()) {
    reply_payload.PackFrom(reply);
    return PayloadOptional(reply_payload);
  }

  StringVector warble_ids_vector = deserialize(warble_ids_str, ',');
  key_vector.clear();
  for (auto s : warble_ids_vector) {
    key_vector.push_back(kWarblePrefix + s);
  }
  StringOptionalVector warbles_opt_vector = kv_store_->Get(key_vector);
  for (auto op : warbles_opt_vector) {
    auto w = reply.add_warbles();
    w->ParseFromString(op.value());
  }

  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}
}  // namespace cs499_fei