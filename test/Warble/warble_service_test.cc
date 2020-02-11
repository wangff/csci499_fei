#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "storage_abstraction.h"
#include "warble_service.h"

using ::testing::Return;

// Mock Class of StorageAbstraction
// Used for dependency injection for Func_platform constructor
class MockStorage: public StorageAbstraction {
 public:
  MOCK_METHOD2(Put, void(const std::string &, const std::string &));
  MOCK_METHOD1(Get, StringOptionalVector(const StringVector&));
  MOCK_METHOD1(Remove, void(const std::string &));
};

// Init the global variables for all the test cases in this test suite
class WarbleTest: public ::testing::Test {
 public:
  WarbleTest(): mock_store_(new MockStorage) , warble_(new Warble(mock_store_)) {}
 protected:
  // dependencies
  std::shared_ptr<MockStorage> mock_store_;
  std::unique_ptr<Warble> warble_;
};

// Test: RegisterUser will initialize warble list, followers list, following list for the new user.
// Exepcted: RegisterUser will call KeyValueStore Put function three times will corresponding key and empty value.
TEST_F(WarbleTest, RegisterUser) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_user_followers_key = "user_followers_user_Harry Potter";
  std::string mock_user_followings_key = "user_followings_user_Harry Potter";
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, ""));
  EXPECT_CALL(*mock_store_, Put(mock_user_followers_key, ""));
  EXPECT_CALL(*mock_store_, Put(mock_user_followings_key, ""));

  std::string user_name = "Harry Potter";
  warble_->RegisterUser(user_name);
}

// Test: user_name follow to_follow when both user_name and to_follow have empty profile
// Expected: Follow call KeyValueStore Put function twice.
//           Put to_follower in user_name's followings list.
//           Put user_name in to_follow's followers list.
TEST_F(WarbleTest, FollowWithEmptyProfile) {
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
TEST_F(WarbleTest, FollowWithProfile) {
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
TEST_F(WarbleTest, ReadProfileWithEmptyProfile) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Harry Potter";
  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};

  StringOptionalVector mock_value_vector = {StringOptional(), StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  auto actual_profile = warble_->ReadProfile("Harry Potter");
  EXPECT_EQ(0,actual_profile.profile_followings.size());
  EXPECT_EQ(0,actual_profile.profile_followers.size());
}

// Test: Read user's profile that is not empty.
// Expected: The return profile is as expected.
TEST_F(WarbleTest, ReadProfile) {
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

  auto actual_profile = warble_->ReadProfile("Harry Potter");
  EXPECT_EQ(expected_profile.profile_followings.size(),expected_profile.profile_followings.size());
  EXPECT_EQ(expected_profile.profile_followers.size(),actual_profile.profile_followers.size());

  for(int i = 0; i < expected_profile.profile_followings.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followings.at(i), actual_profile.profile_followings.at(i));
  }

  for(int i = 0; i < expected_profile.profile_followers.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followers.at(i), actual_profile.profile_followers.at(i));
  }
}