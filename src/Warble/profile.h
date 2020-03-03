#ifndef CSCI499_FEI_SRC_WARBLE_PROFILE_H_
#define CSCI499_FEI_SRC_WARBLE_PROFILE_H_

namespace cs499_fei {
// User's profile of following and followers.
struct Profile {
  StringVector profile_followings;
  StringVector profile_followers;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_WARBLE_PROFILE_H_
