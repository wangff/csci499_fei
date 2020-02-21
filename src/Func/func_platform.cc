#include "func_platform.h"

FuncPlatform::FuncPlatform(const StoragePtr &storage, const WarblePtr &warble)
    : kv_store_(storage), warble_service_(warble), hook_dict_({}) { }

// Register the service to handle function when specific event occur.
void FuncPlatform::Hook(const EventType &event_type, const FunctionType &function_type) {
  hook_dict_[event_type] = function_type;
};

// Unregister event from service, return true if service is unregistered
void FuncPlatform::Unhook(const EventType &event_type) {
  hook_dict_.erase(event_type);
};

// Execute handler function based on event type
Payload FuncPlatform::Execute(const EventType &event_type, const Payload &payload) {
  Payload reply_payload;
  if (!hook_dict_.count(event_type)) {
    return reply_payload;
  }
  std::string function_str = hook_dict_.at(event_type);
  if (function_str.find("register") != std::string::npos) {
    RegisteruserRequest request;
    payload.UnpackTo(&request);
    RegisteruserReply reply;
    std::string user_name = request.username();
    warble_service_->RegisterUser(user_name);
    reply_payload.PackFrom(reply);
  }
  else if (function_str.find("warble") != std::string::npos) {
    timeval time;
    gettimeofday(&time,NULL);

    WarbleRequest request;
    WarbleReply reply;
    payload.UnpackTo(&request);
    std::string user_name = request.username();
    std::string text = request.text();
    std::string reply_to = request.parent_id();
    StringOptional reply_to_opt = StringOptional(reply_to);
    std::string warble_id = warble_service_->WarbleText(user_name, text, reply_to_opt);

    reply.mutable_warble()->set_username(user_name);
    reply.mutable_warble()->set_text(text);
    reply.mutable_warble()->set_id(warble_id);
    reply.mutable_warble()->set_parent_id(reply_to);
    reply.mutable_warble()->mutable_timestamp()->set_seconds(time.tv_sec);
    reply.mutable_warble()->mutable_timestamp()->set_useconds(time.tv_usec);
    reply_payload.PackFrom(reply);
  }
  else if (function_str.find("follow") != std::string::npos) {
    FollowRequest request;
    payload.UnpackTo(&request);
    FollowReply reply;
    std::string user_name = request.username();
    std::string to_follow = request.to_follow();
    warble_service_->Follow(user_name, to_follow);
    reply_payload.PackFrom(reply);
  }
  else if (function_str.find("read") != std::string::npos) {
    ReadRequest request;
    payload.UnpackTo(&request);
    ReadReply reply;
    std::string warble_id = request.warble_id();
    StringVector warbles_as_str = warble_service_->ReadThread(warble_id);
    for (auto s: warbles_as_str) {
      auto w = reply.add_warbles();
      w->ParseFromString(s);
    }
    reply_payload.PackFrom(reply);
  }
  else if (function_str.find("profile") != std::string::npos) {
    ProfileRequest request;
    payload.UnpackTo(&request);
    ProfileReply reply;
    std::string user_name = request.username();
    Profile profile = warble_service_->ReadProfile(user_name);

    for (const auto &following : profile.profile_followings) {
      reply.add_following(following);
    }
    for (const auto &follower : profile.profile_followers) {
      reply.add_followers(follower);
    }
    reply_payload.PackFrom(reply);
  }
  return reply_payload;
}


