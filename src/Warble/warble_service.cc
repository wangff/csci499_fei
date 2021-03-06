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

PayloadOptional WarbleService::RegisterUser(const Payload &payload,
                                            const StoragePtr &kv_store) {
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
  StringOptional user_warbles = kv_store->Get(keys_vector).at(0);

  bool is_user_exist =
      (user_warbles.has_value()) && (!user_warbles.value().empty());

  RegisteruserReply reply;
  Payload reply_payload;
  reply_payload.PackFrom(reply);

  if (is_user_exist) {
    return PayloadOptional();
  }

  kv_store->Put(user_warbles_key, kInit);
  kv_store->Put(user_followers_key, kInit);
  kv_store->Put(user_followings_key, kInit);
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::Follow(const Payload &payload,
                                      const StoragePtr &kv_store) {
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

  StringOptionalVector value_vector = kv_store->Get(key_vector);
  StringOptional user_followings = value_vector.at(0);
  StringOptional to_follow_followers = value_vector.at(1);

  std::string new_to_follow_followers = user_name;
  std::string new_user_followings = to_follow;

  // Check if user_name and to_follow have been registered.
  bool is_user_name_registered =
      (user_followings != std::nullopt) && (!user_followings.value().empty());
  bool is_to_follow_registered = (to_follow_followers != std::nullopt) &&
                                 (!to_follow_followers.value().empty());

  // If either user_name or to_follow has not been registered, following
  // operations will fail.
  if (!is_user_name_registered || !is_to_follow_registered) {
    return PayloadOptional();
  }

  if ((user_followings != std::nullopt) && (user_followings.value() != kInit)) {
    new_user_followings = user_followings.value() + "," + new_user_followings;
  }

  if ((to_follow_followers != std::nullopt) && (to_follow_followers != kInit)) {
    new_to_follow_followers =
        to_follow_followers.value() + "," + new_to_follow_followers;
  }

  kv_store->Put(user_followings_key, new_user_followings);
  kv_store->Put(to_follow_followers_key, new_to_follow_followers);

  FollowReply reply;
  Payload reply_payload;
  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::ReadProfile(const Payload &payload,
                                           const StoragePtr &kv_store) {
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

  StringOptionalVector value_vector = kv_store->Get(key_vector);
  StringOptional user_followings = value_vector.at(0);
  StringOptional user_followers = value_vector.at(1);

  // Check if user_name and to_follow have been registered.
  bool is_user_followings_registered =
      (user_followings != std::nullopt) && (!user_followings.value().empty());
  bool is_user_followers_registered =
      (user_followers != std::nullopt) && (!user_followers.value().empty());

  // If either user_name or to_follow has not been registered, reading profile
  // operations will fail.
  if (!is_user_followings_registered || !is_user_followers_registered) {
    return PayloadOptional();
  }

  Profile profile;

  if ((user_followings != std::nullopt) && (user_followings.value() != kInit)) {
    profile.profile_followings = deserialize(user_followings.value(), ',');
  }

  if ((user_followers != std::nullopt) && (user_followers != kInit)) {
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

PayloadOptional WarbleService::WarbleText(const Payload &payload,
                                          const StoragePtr &kv_store) {
  timeval time;
  gettimeofday(&time, NULL);

  WarbleRequest request;

  payload.UnpackTo(&request);
  std::string user_name = request.username();
  std::string text = request.text();
  std::string reply_to = request.parent_id();
  StringVector hashtag_list = GetHashtagList(text);

  // Create key vector
  // 0: warble list for user_name
  // 1: Optional. warble list for reply_to
  std::string user_warble_key = kUserWarblesPrefix + kUserPrefix + user_name;
  StringVector key_vector = {user_warble_key};
  std::string warble_thread_key;
  if (reply_to != "") {
    warble_thread_key = kWarbleThreadPrefix + kWarblePrefix + reply_to;
    key_vector.push_back(warble_thread_key);

    // Used to check whether reply_to warble exists.
    std::string warble_key = kWarblePrefix + reply_to;
    key_vector.push_back(warble_key);
  }

  StringOptionalVector value_vector = kv_store->Get(key_vector);

  // Check whether user_name has been registered.
  StringOptional user_warbles = value_vector.at(0);

  bool is_user_exist =
      (user_warbles.has_value()) && (!user_warbles.value().empty());

  if (!is_user_exist) {
    return PayloadOptional();
  }

  // Check whether reply_to warble does exist.
  if (reply_to != "") {
    StringOptional warble = value_vector.at(2);

    bool is_warble_exist =
        (warble != std::nullopt) && (!warble.value().empty());

    if (not is_warble_exist) {
      return PayloadOptional();
    }
  }

  uint32_t warble_id = randomID();
  std::string current_warble_id = std::to_string(warble_id);

  Warble new_warble;
  new_warble.set_username(user_name);
  new_warble.set_text(text);
  new_warble.set_id(current_warble_id);
  new_warble.set_parent_id(reply_to);
  new_warble.mutable_timestamp()->set_seconds(time.tv_sec);
  new_warble.mutable_timestamp()->set_useconds(time.tv_usec);

  std::string warble_key = kWarblePrefix + current_warble_id;
  kv_store->Put(warble_key, new_warble.SerializeAsString());

  std::string new_user_warbles = current_warble_id;
  if ((user_warbles != std::nullopt) && (user_warbles.value() != kInit)) {
    new_user_warbles = user_warbles.value() + "," + new_user_warbles;
  }

  kv_store->Put(user_warble_key, new_user_warbles);

  // Put {hashtag, Warble} pair to kv_store
  for (int i = 0; i < hashtag_list.size(); i++) {
    std::string hashtag_key = kHashtagPrefix + hashtag_list[i];
    StringVector k = {hashtag_key};
    StringOptionalVector v = kv_store->Get(k);
    StringOptional value = std::nullopt;
    if (v.size() > 0) {
      value = v[0];
    }
    std::string id_list = current_warble_id;
    if (value != std::nullopt && value.value() != "") {
      id_list = value.value() + "," + id_list;
    }
    kv_store->Put(hashtag_key, id_list);
  }

  if (reply_to != "") {
    StringOptional warble_thread = value_vector.at(1);
    std::string new_warble_thread = current_warble_id;
    if ((warble_thread != std::nullopt) && (warble_thread.value() != "")) {
      new_warble_thread = warble_thread.value() + "," + new_warble_thread;
    }
    kv_store->Put(warble_thread_key, new_warble_thread);
  }

  WarbleReply reply;
  reply.mutable_warble()->CopyFrom(new_warble);

  Payload reply_payload;
  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::ReadThread(const Payload &payload,
                                          const StoragePtr &kv_store) {
  ReadRequest request;
  payload.UnpackTo(&request);
  std::string warble_id = request.warble_id();

  StringVector warbles_str_vector;
  std::string warble_thread_key =
      kWarbleThreadPrefix + kWarblePrefix + warble_id;
  std::string warble_key = kWarblePrefix + warble_id;
  StringVector key_vector = {warble_thread_key, warble_key};
  StringOptionalVector value_vector = kv_store->Get(key_vector);
  StringOptional warble_ids_opt = value_vector.at(0);
  StringOptional warble = value_vector.at(1);

  // Check if this warble exists.
  bool is_warble_exist = (warble != std::nullopt) && (!warble.value().empty());

  if (not is_warble_exist) {
    return PayloadOptional();
  }

  ReadReply reply;
  Payload reply_payload;

  // Add the start warble to the reply
  auto w = reply.add_warbles();
  w->ParseFromString(warble.value());

  std::string warble_ids_str = "";
  if (warble_ids_opt != std::nullopt && warble_ids_opt.has_value()) {
    warble_ids_str = warble_ids_opt.value();
  }

  if (warble_ids_str.empty()) {
    reply_payload.PackFrom(reply);
    return PayloadOptional(reply_payload);
  }

  StringVector warble_ids_vector = deserialize(warble_ids_str, ',');
  key_vector.clear();

  for (auto s : warble_ids_vector) {
    key_vector.push_back(kWarblePrefix + s);
  }
  StringOptionalVector warbles_opt_vector = kv_store->Get(key_vector);
  for (auto op : warbles_opt_vector) {
    auto w = reply.add_warbles();
    w->ParseFromString(op.value());
  }

  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}

PayloadOptional WarbleService::Stream(const Payload &payload, 
                                       const StoragePtr &kv_store) {
  StreamRequest request;
  StreamReply reply;
  Payload reply_payload;
  StringVector key_vector;
  payload.UnpackTo(&request);
  std::string hashtag_key = kHashtagPrefix + request.hashtag();;
  Timestamp time = request.time();
  int startTime = time.seconds();
  
  StringVector k = {hashtag_key};
  std::string string_ids = "";
  StringOptionalVector res = kv_store->Get(k);
  if (res.size() == 0) {
    return PayloadOptional();
  } else {
    string_ids = res[0].value_or("");
  }
  if (string_ids != "") {
    StringVector vector_ids = deserialize(string_ids, ',');
    for (const auto& s : vector_ids) {
      key_vector.push_back(kWarblePrefix + s);
    }
    StringOptionalVector warbles_opt_vector = kv_store->Get(key_vector);
    LOG(INFO) << "The number of warbles containing hastag:" << warbles_opt_vector.size()<<std::endl;
    if (warbles_opt_vector.size() > 0) {
      for (const auto& op : warbles_opt_vector) {
        Warble temp;
        temp.ParseFromString(op.value());
        int w_time = temp.timestamp().seconds();
        if (w_time > startTime) {
          Warble* w = reply.add_warbles();
          w->ParseFromString(op.value());
        }
      }
    }
  }
  reply_payload.PackFrom(reply);
  return PayloadOptional(reply_payload);
}

StringVector WarbleService::GetHashtagList(std::string text) {
  StringVector list;
  int i = 0;
  while (i < text.length()) {
    if(text.at(i) == '#') {
      i++;
      int start = i;
      while (i < text.length() && text.at(i) != ' ') {
        i++;
      }
      int end = i;
      list.push_back(text.substr(start, end-start));
    }
    i++;
  }
  return list;
}

}  // namespace cs499_fei