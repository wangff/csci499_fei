#include <chrono>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

#include <glog/logging.h>

#include "func_service_client.h"
#include "user_interface_cmd.h"
#include "Warble.grpc.pb.h"

using warble::FollowRequest;
using warble::ProfileReply;
using warble::ProfileRequest;
using warble::ReadReply;
using warble::ReadRequest;
using warble::RegisteruserRequest;
using warble::Timestamp;
using warble::Warble;
using warble::WarbleReply;
using warble::WarbleRequest;
using warble::StreamRequest;
using warble::StreamReply;

using cs499_fei::FLAGS_follow;
using cs499_fei::FLAGS_hook;
using cs499_fei::FLAGS_profile;
using cs499_fei::FLAGS_read;
using cs499_fei::FLAGS_registeruser;
using cs499_fei::FLAGS_reply;
using cs499_fei::FLAGS_unhook;
using cs499_fei::FLAGS_user;
using cs499_fei::FLAGS_warble;
using cs499_fei::FLAGS_stream;
using cs499_fei::FuncServiceClient;

// Helper function: Log with glog and print in console
void logAndPrint(const std::string& s) {
  LOG(INFO) << s;
  std::cout << s;
}

// Helper function: split string by delimiter
std::vector<std::string> split(const std::string& s, char delim) {
  std::istringstream iss(s);
  std::string item;
  std::vector<std::string> res;
  while (getline(iss, item, delim)) {
    res.push_back(item);
  }
  return res;
}

int main(int argc, char** argv) {
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  // Parse command line flags
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  //  FLAGS_alsologtostderr = 1;

  FuncServiceClient func_service_client(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));

  bool flag_hook_not_set =
      gflags::GetCommandLineFlagInfoOrDie("hook").is_default;
  bool flag_unhook_not_set =
      gflags::GetCommandLineFlagInfoOrDie("unhook").is_default;
  bool flag_registeruser_not_set =
      gflags::GetCommandLineFlagInfoOrDie("registeruser").is_default;
  bool flag_user_not_set =
      gflags::GetCommandLineFlagInfoOrDie("user").is_default;
  bool flag_warble_not_set =
      gflags::GetCommandLineFlagInfoOrDie("warble").is_default;
  bool flag_reply_not_set =
      gflags::GetCommandLineFlagInfoOrDie("reply").is_default;
  bool flag_follow_not_set =
      gflags::GetCommandLineFlagInfoOrDie("follow").is_default;
  bool flag_read_not_set =
      gflags::GetCommandLineFlagInfoOrDie("read").is_default;
  bool flag_profile_not_set =
      gflags::GetCommandLineFlagInfoOrDie("profile").is_default;
  bool flag_stream_not_set =
      gflags::GetCommandLineFlagInfoOrDie("stream").is_default;

  // ./warble --hook "event type:function str"
  if (!flag_hook_not_set) {
    auto splited_vector = split(FLAGS_hook, ':');
    auto event_type = std::stoi(splited_vector.at(0));
    auto function_str = splited_vector.at(1);
    func_service_client.Hook(event_type, function_str);
    std::string output_str =
        "Hook " + std::to_string(event_type) + ":" + function_str + ".\n";
    logAndPrint(output_str);
  }
  // ./warble --unhook "event type"
  else if (!flag_unhook_not_set) {
    auto event_type = std::stoi(FLAGS_unhook);
    func_service_client.UnHook(event_type);
    std::string output_str = "UnHook " + std::to_string(event_type) + ".\n";
    logAndPrint(output_str);
  }
  // ./warble --registeruser "username"
  else if (!flag_registeruser_not_set) {
    RegisteruserRequest request;
    request.set_username(FLAGS_registeruser);
    Payload payload;
    payload.PackFrom(request);
    int event_type = 1;
    OptionalPayload res_payload_opt =
        func_service_client.Event(event_type, &payload);
    std::string output_str;
    if (res_payload_opt.has_value()) {
      output_str = "Register user: " + FLAGS_registeruser + ".\n";
    } else {
      output_str = "Registration failed.\n";
    }
    logAndPrint(output_str);
  } 
  // ./warble --user "username" --warble "warble content"
  // ./warble --user "username" --warble "warble content" --reply "reply to
  // id"
  else if (!flag_user_not_set and !flag_warble_not_set) {
    int event_type = 2;
    Payload payload;
    WarbleRequest request;
    request.set_username(FLAGS_user);
    request.set_text(FLAGS_warble);
    std::string output_str =
        "User: " + FLAGS_user + "\nwarble: " + FLAGS_warble + ".\n";
    logAndPrint(output_str);
    if (!flag_reply_not_set) {
      request.set_parent_id(FLAGS_reply);
      output_str = "This warble reply: " + FLAGS_reply + ".\n";
      logAndPrint(output_str);
    }
    payload.PackFrom(request);
    OptionalPayload res_payload_opt =
        func_service_client.Event(event_type, &payload);

    if (!res_payload_opt.has_value()) {
      output_str =
          "Warbling a text failed. Either the user or the reply_to warble does "
          "not exist. \n";
      logAndPrint(output_str);
      exit(0);
    }

    Payload res_payload = res_payload_opt.value();

    WarbleReply reply;
    res_payload.UnpackTo(&reply);

    Warble warble = reply.warble();

    output_str = "Warble has been stored successfully. \nusername: " +
                 warble.username() + "\n" + "warble id: " + warble.id() + "\n" +
                 "warble text: " + warble.text() + "\n" +
                 "parent id: " + warble.parent_id() + "\n" +
                 "at time: " + std::to_string(warble.timestamp().seconds()) +
                 ".\n";
    logAndPrint(output_str);
  }
  // ./warble --user "username" --follow "follow_to_user"
  else if (!flag_user_not_set and !flag_follow_not_set) {
    int event_type = 3;
    FollowRequest request;
    request.set_username(FLAGS_user);
    request.set_to_follow(FLAGS_follow);
    Payload payload;
    payload.PackFrom(request);
    OptionalPayload res_payload_opt =
        func_service_client.Event(event_type, &payload);
    std::string output_str;

    if (!res_payload_opt.has_value()) {
      output_str =
          "Following failed! Either followee or follower may not exist! \n";
    } else {
      output_str =
          "User: " + FLAGS_user + " follow: " + "User: " + FLAGS_follow + ".\n";
    }
    logAndPrint(output_str);
  }
  // ./warble --read "The ID of the warble to start the read at."
  else if (!flag_read_not_set) {
    int event_type = 4;
    std::string output_str;
    std::string warble_id = FLAGS_read;
    ReadRequest request;
    request.set_warble_id(warble_id);
    Payload payload;
    payload.PackFrom(request);
    OptionalPayload res_payload_opt =
        func_service_client.Event(event_type, &payload);

    if (!res_payload_opt.has_value()) {
      output_str = "Reading thread of warble " + warble_id +
                   " failed. The warble ID does not exist. \n";
      logAndPrint(output_str);
      exit(0);
    }

    Payload res_payload = res_payload_opt.value();
    ReadReply reply;
    res_payload.UnpackTo(&reply);

    output_str = "Reads the warble thread starting at " + FLAGS_read + ".\n";
    logAndPrint(output_str);
    for (int i = 0; i < reply.warbles_size(); i++) {
      Warble warble = reply.warbles(i);
      output_str = "Warble Id: " + warble.id() +
                   "; User: " + warble.username() +
                   "; Warble Text: " + warble.text();
      if (i == 0) {
        output_str += "\n";
      } else {
        output_str += "; Warble Reply To " + warble.parent_id() + ".\n";
      }
      logAndPrint(output_str);
    }
  }
  // ./warble --user "username" --profile
  else if (!flag_user_not_set and !flag_profile_not_set) {
    int event_type = 5;
    std::string output_str;
    ProfileRequest request;
    request.set_username(FLAGS_user);
    Payload payload;
    payload.PackFrom(request);
    OptionalPayload res_payload_opt =
        func_service_client.Event(event_type, &payload);

    if (!res_payload_opt.has_value()) {
      output_str =
          "Getting user's profile failed. The user has not been registered. \n";
      logAndPrint(output_str);
      exit(0);
    }

    Payload res_payload = res_payload_opt.value();

    ProfileReply reply;
    res_payload.UnpackTo(&reply);
    auto followers = reply.followers();
    auto followings = reply.following();
    output_str = "User: " + FLAGS_user + " has followers :\n";
    logAndPrint(output_str);
    for (const auto& follower : followers) {
      logAndPrint(follower + "\n");
    }
    output_str = "User: " + FLAGS_user + " has followings :\n";
    logAndPrint(output_str);
    for (const auto& following : followings) {
      logAndPrint(following + "\n");
    }
  }
  // ./warble --stream <hashtag> 
  else if (!flag_stream_not_set) {
    std::string output = "Stream all new warbles containing #" + FLAGS_stream +":\n";
    logAndPrint(output);
    StreamRequest request;
    auto now = std::chrono::system_clock::now().time_since_epoch();
    Timestamp* time = new Timestamp();
    time->set_seconds(std::chrono::duration_cast<std::chrono::seconds>(now).count());
    time->set_useconds(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
    request.set_allocated_time(time);
    request.set_hashtag(FLAGS_stream);

    while (true) {
      Payload payload;
      payload.PackFrom(request);
      int event_type = 6;
      OptionalPayload res_payload_opt = func_service_client.Event(event_type, &payload);
      if (res_payload_opt.has_value()) {
        StreamReply reply;
        res_payload_opt.value().UnpackTo(&reply);
        for (int i = 0; i < reply.warbles_size(); i++) {
          Warble warble = reply.warbles(i);
          int seconds = warble.timestamp().seconds();
          std::time_t t = static_cast<std::time_t>(seconds);
          std::string time_string(std::asctime(std::localtime(&t)));
          std::string output_str = "Warble Id: " + warble.id() 
                                 + "; User: " + warble.username() 
                                 + "; Warble Text: " + warble.text()
                                 + "; Time: " +  time_string ;
          logAndPrint(output_str);
        }
        if (reply.warbles_size() > 0) {
          Timestamp latest = reply.warbles((reply.warbles_size() - 1)).timestamp();
          request.mutable_time()->CopyFrom(latest);
        }
      }
      //refresh every 5s
      std::this_thread::sleep_for(std::chrono::seconds(5));
    }
  } else {
    std::string output_str =
        "Invalid commad; \nPlease refer to ./warble --help.\n";
    logAndPrint(output_str);
  }
}