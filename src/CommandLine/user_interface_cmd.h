#ifndef CSCI499_FEI_SRC_FRONTEND_USER_INTERFACE_CMD_H_
#define CSCI499_FEI_SRC_FRONTEND_USER_INTERFACE_CMD_H_

#include <gflags/gflags.h>

namespace cs499_fei {
DEFINE_string(hook, "event type:function string",
              "register the mapping relationship between event type and "
              "function on Func");
DEFINE_string(
    unhook, "event type",
    "remove the mapping relationship between event type and function on Func");
DEFINE_string(registeruser, "username", "Registers the given username");
DEFINE_string(user, "username", "Logs in as the given username");
DEFINE_string(warble, "warble text",
              "Creates a new warble with the given text");
DEFINE_string(reply, "warble_id",
              "Indicates that the new warble is a reply to the given id");
DEFINE_string(follow, "username", "Starts following the given username");
DEFINE_string(read, "warble_id",
              "Reads the warble thread starting at the given id");
DEFINE_string(profile, "",
              "Gets the user’s profile of following and followers");
}// namespace cs499_fei
#endif //CSCI499_FEI_SRC_FRONTEND_USER_INTERFACE_CMD_H_
