#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <sys/time.h>

#include <glog/logging.h>

#include "func_service_client.h"
#include "user_interface_cmd.h"
#include "Warble.grpc.pb.h"

using warble::Timestamp;
using warble::Warble;
using warble::RegisteruserRequest;
using warble::WarbleRequest;
using warble::WarbleReply;
using warble::FollowRequest;
using warble::ReadRequest;
using warble::ReadReply;
using warble::ProfileRequest;
using warble::ProfileReply;

// Helper function: Log with glog and print in console
void logAndPrint(const std::string &s) {
  LOG(INFO) << s;
  std::cout << s;
}

// Helper function: split string by delimiter
std::vector<std::string> split(const std::string &s, char delim) {
  std::istringstream iss(s);
  std::string item;
  std::vector<std::string> res;
  while(getline(iss,item,delim)) {
    res.push_back(item);
  }
  return res;
}

int main(int argc, char **argv)
{
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  // Parse command line flags
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  FuncServiceClient func_service_client(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));

  bool flag_hook_not_set = gflags::GetCommandLineFlagInfoOrDie("hook").is_default;
  bool flag_unhook_not_set = gflags::GetCommandLineFlagInfoOrDie("unhook").is_default;
  bool flag_resgisteruser_not_set = gflags::GetCommandLineFlagInfoOrDie("resgisteruser").is_default;
  bool flag_user_not_set = gflags::GetCommandLineFlagInfoOrDie("user").is_default;
  bool flag_warble_not_set = gflags::GetCommandLineFlagInfoOrDie("warble").is_default;
  bool flag_reply_not_set = gflags::GetCommandLineFlagInfoOrDie("reply").is_default;
  bool flag_follow_not_set = gflags::GetCommandLineFlagInfoOrDie("follow").is_default;
  bool flag_read_not_set = gflags::GetCommandLineFlagInfoOrDie("read").is_default;
  bool flag_profile_not_set = gflags::GetCommandLineFlagInfoOrDie("profile").is_default;

  std::cout << "Hello Warble.\n";
  // ./Warble --hook "event type:function str"
  if (!flag_hook_not_set) {
    auto splited_vector = split(FLAGS_hook, ':');
    auto event_type = std::stoi(splited_vector.at(0));
    auto function_str = splited_vector.at(1);
    func_service_client.Hook(event_type,function_str);
    std::string output_str = "Hook " + std::to_string(event_type) + ":" + function_str +".\n";
    logAndPrint(output_str);
  }
  // ./Warble --unhook "event type"
  else if (!flag_unhook_not_set) {
    auto event_type = std::stoi(FLAGS_unhook);
    func_service_client.UnHook(event_type);
    std::string output_str = "UnHook " + std::to_string(event_type) + ".\n";
    logAndPrint(output_str);
  }
  // ./Warble --registeruser "username"
  else if (!flag_resgisteruser_not_set) {
    RegisteruserRequest request;
    request.set_username(FLAGS_resgisteruser);
    Any payload;
    payload.PackFrom(request);
    int event_type = 1;
    func_service_client.Event(event_type, &payload);
    std::string output_str = "Register user: " + FLAGS_resgisteruser + ".\n";
    logAndPrint(output_str);
  }
  // ./Warble --user "username" --warble "warble content"
  // ./Warble --user "username" --warble "warble content" --parent_id "reply to id"
  else if (!flag_user_not_set and !flag_warble_not_set) {
    int event_type = 2;
    Any payload;
    WarbleRequest request;
    request.set_username(FLAGS_user);
    request.set_text(FLAGS_warble);
    std::string output_str = "User: " + FLAGS_user + "warble: " + FLAGS_warble + ".\n";
    logAndPrint(output_str);
    if(!flag_reply_not_set) {
      request.set_parent_id(FLAGS_reply);
      std::string output_str = "This warble reply: " + FLAGS_reply + ".\n";
      logAndPrint(output_str);
    }
    payload.PackFrom(request);
    Any res_payload = func_service_client.Event(event_type, &payload);
    Warble reply;
    res_payload.UnpackTo(&reply);
    output_str =  "Warble has been stored successfully. \n username: " + reply.username() + "\n"
                + "warble id: " + reply.id() + "\n"
                + "warble text: " + reply.text() + "\n"
                + "parent id: " + reply.parent_id() + "\n"
                + "at time: " + std::to_string(reply.timestamp().seconds()) + ".\n";
    logAndPrint(output_str);
  }
  // ./Warble --user "username" --follow "follow_to_user"
  else if (!flag_user_not_set and !flag_follow_not_set) {
    int event_type = 3;
    FollowRequest request;
    request.set_username(FLAGS_user);
    request.set_to_follow(FLAGS_follow);
    Any payload;
    payload.PackFrom(request);
    func_service_client.Event(event_type, &payload);
    std::string output_str = "User: " + FLAGS_user + "follow: " + "User: " + FLAGS_follow + ".\n";
    logAndPrint(output_str);
  }
  // ./Warble --read "The ID of the warble to start the read at."
  else if (!flag_read_not_set) {
    int event_type = 4;
    ReadRequest request;
    request.set_warble_id(FLAGS_read);
    Any payload;
    payload.PackFrom(request);
    Any res_payload = func_service_client.Event(event_type, &payload);
    ReadReply reply;
    res_payload.UnpackTo(&reply);
    std::string output_str =  "Reads the warble thread starting at " + FLAGS_read + ".\n";
    logAndPrint(output_str);
    for (const auto& warble : reply.warbles()) {
      output_str = "User: "+ warble.username() + "; Warble Number: " + warble.id() + "; Warble Text: " + warble.text() + ".\n";
      logAndPrint(output_str);
    }
  }
  // ./Warble --user "username" --profile
  else if (!flag_user_not_set and !flag_profile_not_set) {
    int event_type = 5;
    ProfileRequest request;
    request.set_username(FLAGS_user);
    Any payload;
    payload.PackFrom(request);
    Any res_payload = func_service_client.Event(event_type, &payload);
    ProfileReply reply;
    res_payload.UnpackTo(&reply);
    auto followers = reply.followers();
    auto followings = reply.following();
    std::string output_str =  "User: " + FLAGS_user + "has followers :\n";
    logAndPrint(output_str);
    for (const auto& follower : followers) {
      logAndPrint(follower);
    }
    output_str = "User: " + FLAGS_user + "has followings :\n";
    logAndPrint(output_str);
    for (const auto& following : followings) {
      logAndPrint(following);
    }
  }
}