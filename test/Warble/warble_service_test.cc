#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "storage_abstraction.h"
#include "warble_service.h"

using ::testing::Return;

// Mock Class of StorageAbstraction
// Used for dependency injection for Warble constructor
class MockStorage: public StorageAbstraction {
 public:
  MOCK_METHOD2(Put, void(const std::string &, const std::string &));
  MOCK_METHOD1(Get, StringOptionalVector(const StringVector&));
  MOCK_METHOD1(Remove, void(const std::string &));
};

// Init the global variables for all the test cases in this test suite
class WarbleTest: public ::testing::Test {
 public:
  WarbleTest(): mock_store_(new MockStorage) , warble_(new WarbleService(mock_store_)) {}
 protected:
  // dependencies
  std::shared_ptr<MockStorage> mock_store_;
  std::unique_ptr<WarbleService> warble_;
};

// Test: RegisterUser successfully initializes warble list, followers list, following list for the new user.
//       and the username has not been registered.
// Expected: RegisterUser will call KeyValueStore Put function three times will corresponding key and empty value.
//           RegisterUser return value true.
TEST_F(WarbleTest, shouldSucceedWhenRegisterANewUserName) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_user_followers_key = "user_followers_user_Harry Potter";
  std::string mock_user_followings_key = "user_followings_user_Harry Potter";

  StringVector mock_key_vector = {mock_user_warbles_key};
  StringOptionalVector mock_user_warbles_value = {StringOptional()};
  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
             .Times(1)
             .WillOnce(Return(mock_user_warbles_value));
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, ""));
  EXPECT_CALL(*mock_store_, Put(mock_user_followers_key, ""));
  EXPECT_CALL(*mock_store_, Put(mock_user_followings_key, ""));
  std::string user_name = "Harry Potter";
  bool is_register_succeed = warble_->RegisterUser(user_name);
  EXPECT_TRUE(is_register_succeed);
}

// Test: RegisterUser failed since duplicate username.
// Expected: RegisterUser return value false.
TEST_F(WarbleTest, shouldFailWhenRegisterADuplicateUserName) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_user_followers_key = "user_followers_user_Harry Potter";
  std::string mock_user_followings_key = "user_followings_user_Harry Potter";

  StringVector mock_key_vector = {mock_user_warbles_key};
  StringOptionalVector mock_user_warbles_value = {"Harry Potter"};
  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_user_warbles_value));
  std::string user_name = "Harry Potter";
  bool is_register_succeed = warble_->RegisterUser(user_name);
  EXPECT_FALSE(is_register_succeed);
}

// Test: user_name follow to_follow when both user_name and to_follow have empty profile
// Expected: Follow call KeyValueStore Put function twice.
//           Put to_follower in user_name's followings list.
//           Put user_name in to_follow's followers list.
TEST_F(WarbleTest, shouldAppendCurrentUserAndToFollowToEachOtherProfileWhenCurrentUserFollowToFollowAndBothOFThemHasNoProfiles) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Lord Voldmort";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {StringOptional(), StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
            .Times(1)
            .WillOnce(Return(mock_value_vector));

  std::string mock_user_followings = "Lord Voldmort";
  std::string mock_to_follow_followers = "Harry Potter";
  EXPECT_CALL(*mock_store_, Put(user_followings_key, mock_user_followings)).Times(1);
  EXPECT_CALL(*mock_store_, Put(to_follow_followers_key,mock_to_follow_followers)).Times(1);
  warble_->Follow("Harry Potter", "Lord Voldmort");
}

// Test: user_name follow to_follow when both user_name and to_follow have profile
// Expected: Follow call KeyValueStore Put function twice.
//           Put to_follower in user_name's followings list.
//           Put user_name in to_follow's followers list.
TEST_F(WarbleTest, shouldAppendCurrentUserAndToFollowToEachOtherProfileWhenCurrentUserFollowToFollowAndBothOFThemHasProfiles) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Lord Voldmort";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {"following1,following2,following3", "follower1,follower2,follower3"};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  std::string mock_user_followings = "following1,following2,following3,Lord Voldmort";
  std::string mock_to_follow_followers = "follower1,follower2,follower3,Harry Potter";
  EXPECT_CALL(*mock_store_, Put(user_followings_key, mock_user_followings)).Times(1);
  EXPECT_CALL(*mock_store_, Put(to_follow_followers_key,mock_to_follow_followers)).Times(1);
  warble_->Follow("Harry Potter", "Lord Voldmort");
}

// Test: Read user's profile that is empty.
// Expected: The return profile with empty followings vector and followers vector.
TEST_F(WarbleTest, shouldReturnEmptyProfileWhenReadProfileOfAUserWithoutAnyFollowerOrFollowing) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Harry Potter";
  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};

  StringOptionalVector mock_value_vector = {StringOptional(), StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  Profile actual_profile = warble_->ReadProfile("Harry Potter");
  EXPECT_EQ(0,actual_profile.profile_followings.size());
  EXPECT_EQ(0,actual_profile.profile_followers.size());
}

// Test: Read user's profile that is not empty.
// Expected: The return profile is as expected.
TEST_F(WarbleTest, shouldReturnProfileWhenReadProfileOfAUserWithAnyFollowerOrFollowing) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Harry Potter";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {"following1,following2,following3", "follower1,follower2,follower3"};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  Profile expected_profile;
  expected_profile.profile_followings.push_back("following1");
  expected_profile.profile_followings.push_back("following2");
  expected_profile.profile_followings.push_back("following3");
  expected_profile.profile_followers.push_back("follower1");
  expected_profile.profile_followers.push_back("follower2");
  expected_profile.profile_followers.push_back("follower3");

  Profile actual_profile = warble_->ReadProfile("Harry Potter");
  EXPECT_EQ(expected_profile.profile_followings.size(),expected_profile.profile_followings.size());
  EXPECT_EQ(expected_profile.profile_followers.size(),actual_profile.profile_followers.size());

  for(int i = 0; i < expected_profile.profile_followings.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followings.at(i), actual_profile.profile_followings.at(i));
  }

  for(int i = 0; i < expected_profile.profile_followers.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followers.at(i), actual_profile.profile_followers.at(i));
  }
}

// Test: a user warbles his/her first warble without reply to any warbles.
// Expected: 1. Put a new record for this warble: key = "warble_1", value = "It's my first warble.".
//           2. Append this warble id to the user_warbles list: key = "user_warbles_user_Harry Potter", value = "1";
TEST_F(WarbleTest, shouldReturnNewWarbleIdWhenUserWarbleTheFirstWarbleWithoutReplyTo) {
  //mock get warble id
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  StringVector key_vector = {mock_user_warbles_key};

  StringOptionalVector mock_value_vector = {StringOptional()};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  std::string text = "It's my first warble.";

  EXPECT_CALL(*mock_store_, Put("warble_1", text)).Times(1);
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "1")).Times(1);

  std::string warble_id = warble_->WarbleText("Harry Potter", "It's my first warble.", StringOptional());
  EXPECT_EQ(warble_id, "1");
}

// Test: a user warbles a new warble that is not his/her first warble,
//       and this new warble is the reply to a current warble without any replies.
// Expected: 1. Put a new record for this warble: key = "warble_100", value = "It's my second warble.".
//           2. Append this warble id to the user_warbles list: key = "user_warbles_user_Harry Potter", value = "1,100";
//           3. Append this warble id to the warble_thread list: key = "warble_thread_warble_3", value = "100"
TEST_F(WarbleTest, shouldReturnNewWarbleIdWhenWarbleATextReplyToAnotherWarbleWithouReplies) {
  //mock warble id
  warble_->warble_id_ = 100;
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_warble_thread_key = "warble_thread_warble_3";
  StringVector key_vector = {mock_user_warbles_key, mock_warble_thread_key};

  StringOptionalVector mock_value_vector = {"1", StringOptional()};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  std::string text = "It's my second warble.";

  EXPECT_CALL(*mock_store_, Put("warble_100", text)).Times(1);
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "1,100")).Times(1);
  EXPECT_CALL(*mock_store_, Put(mock_warble_thread_key, "100")).Times(1);

  std::string warble_id = warble_->WarbleText("Harry Potter", "It's my second warble.", StringOptional("3"));
  EXPECT_EQ(warble_id, "100");
}

// Test: a user warbles a new warble that is not his/her first warble,
//       and this new warble is the reply to a current warble with some replies.
// Expected: 1. Put a new record for this warble: key = "warble_100", value = "It's my second warble.".
//           2. Append this warble id to the user_warbles list: key = "user_warbles_user_Harry Potter", value = "1,100";
//           3. Append this warble id to the warble_thread list: key = "warble_thread_warble_3", value = "4,5,6,100"
TEST_F(WarbleTest, shouldReturnNewWarbleIdWhenWarbleATextToReplyToAnotherWarbleWithReplies) {
  //mock warble id
  warble_->warble_id_ = 100;
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_warble_thread_key = "warble_thread_warble_3";
  StringVector key_vector = {mock_user_warbles_key, mock_warble_thread_key};

  StringOptionalVector mock_value_vector = {"1", "4,5,6"};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  std::string text = "It's my second warble.";

  EXPECT_CALL(*mock_store_, Put("warble_100", text)).Times(1);
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "1,100")).Times(1);
  EXPECT_CALL(*mock_store_, Put(mock_warble_thread_key, "4,5,6,100")).Times(1);

  std::string warble_id = warble_->WarbleText("Harry Potter", "It's my second warble.", StringOptional("3"));
  EXPECT_EQ(warble_id, "100");
}

// Test: ReadThread of a warble with id "123" that has no replies.
// Expected: Expect call KeyValueStore Get function once with the key "warble_thread_warble_123"
//           ReadThread function return the empty vector of strings
TEST_F(WarbleTest, shouldReturnEmptyStringVectorWhenReadThreadOfWarbleWithoutAnyReplies) {
  std::string mock_warble_thread_key = "warble_thread_warble_123";
  StringVector mock_key_vector = {mock_warble_thread_key};
  StringOptionalVector mock_value_vector = {StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
         .Times(1)
         .WillOnce(Return(mock_value_vector));

  StringVector warbles = warble_->ReadThread("123");
  EXPECT_TRUE(warbles.empty());
}

// Test: ReadThread of a warble with id "123" that has replies.
// Expected: Expect call KeyValueStore Get function twice
//           First call with the input key vector {"warble_thread_warble_123"}
//           Second call with the input key vecotr {"warble_1", "warble_2", "warble_3"}
//           ReadThread function return the vector within three strings
TEST_F(WarbleTest, shouldReturnTheStringVectorOfRepliesWhenReadThreadOfWarbleHasSomeReplies) {
  std::string mock_warble_thread_key = "warble_thread_warble_123";
  StringVector mock_key_vector = {mock_warble_thread_key};
  StringOptionalVector mock_value_vector = {"1,2,3"};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  mock_key_vector.clear();
  mock_key_vector.push_back("warble_1");
  mock_key_vector.push_back("warble_2");
  mock_key_vector.push_back("warble_3");

  mock_value_vector.clear();
  mock_value_vector.push_back("It is the first warble.");
  mock_value_vector.push_back("It is the second warble.");
  mock_value_vector.push_back("It is the third warble.");

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  StringVector warbles = warble_->ReadThread("123");
  EXPECT_EQ(warbles.size(), 3);
  EXPECT_EQ(warbles.at(0), "It is the first warble.");
  EXPECT_EQ(warbles.at(1), "It is the second warble.");
  EXPECT_EQ(warbles.at(2), "It is the third warble.");
}
