#include "warble_service.h"

#include <sys/time.h>

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "storage_abstraction.h"

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
TEST_F(WarbleTest, shouldInitializeThreeListsWhenRegister) {
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

  Payload mock_payload;
  RegisteruserRequest mock_request;
  mock_request.set_username("Harry Potter");
  mock_payload.PackFrom(mock_request);
  Payload reply_payload = warble_->RegisterUser(mock_payload);
}

// Test: user_name follow to_follow when both user_name and to_follow have empty profile
// Expected: Follow call KeyValueStore Put function twice.
//           Put to_follower in user_name's followings list.
//           Put user_name in to_follow's followers list.
TEST_F(WarbleTest, shouldAddTheFirstFollowerAndFollowingWhenCallFollowFirstTime) {
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

  Payload mock_payload;
  FollowRequest request;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  mock_payload.PackFrom(request);
  Payload reply_payload = warble_->Follow(mock_payload);
}

// Test: user_name follow to_follow when both user_name and to_follow have profile
// Expected: Follow call KeyValueStore Put function twice.
//           Put to_follower in user_name's followings list.
//           Put user_name in to_follow's followers list.
TEST_F(WarbleTest, shouldAppendNewFollowingAndFollowWhenCallFollow) {
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

  Payload mock_payload;
  FollowRequest request;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  mock_payload.PackFrom(request);
  Payload reply_payload = warble_->Follow(mock_payload);
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

  Payload mock_payload;
  ProfileRequest request;
  request.set_username("Harry Potter");
  mock_payload.PackFrom(request);
  Payload reply_payload = warble_->ReadProfile(mock_payload);

  ProfileReply reply;
  reply_payload.UnpackTo(&reply);

  auto actual_profile_followings = reply.following();
  auto actual_profile_followers = reply.followers();

  EXPECT_EQ(0,actual_profile_followings.size());
  EXPECT_EQ(0,actual_profile_followers.size());
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

  Payload mock_payload;
  ProfileRequest request;
  request.set_username("Harry Potter");
  mock_payload.PackFrom(request);
  Payload reply_payload = warble_->ReadProfile(mock_payload);

  ProfileReply reply;
  reply_payload.UnpackTo(&reply);

  auto actual_profile_followings = reply.following();
  auto actual_profile_followers = reply.followers();

  EXPECT_EQ(expected_profile.profile_followings.size(),actual_profile_followings.size());
  EXPECT_EQ(expected_profile.profile_followers.size(),actual_profile_followers.size());

  for(int i = 0; i < expected_profile.profile_followings.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followings.at(i), actual_profile_followings.Get(i));
  }

  for(int i = 0; i < expected_profile.profile_followers.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followers.at(i), actual_profile_followers.Get(i));
  }
}

// Test: a user warbles his/her first warble without reply to any warbles.
// Expected: 1. Put a new record for this warble: key = "warble_1", value = "It's my first warble.".
//           2. Append this warble id to the user_warbles list: key = "user_warbles_user_Harry Potter", value = "1";
TEST_F(WarbleTest, shouldReturnNewWarbleWhenUserWarbleTheFirstWarbleWithoutReplyTo) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  StringVector key_vector = {mock_user_warbles_key};

  StringOptionalVector mock_value_vector = {StringOptional()};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .Times(1)
      .WillOnce(Return(mock_value_vector));

  std::string text = "It's my first warble.";

  EXPECT_CALL(*mock_store_, Put("warble_1", text)).Times(1);
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "1")).Times(1);

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my first warble.");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  Payload reply_payload = warble_->WarbleText(mock_payload);

  WarbleReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(reply.mutable_warble()->username(),request.username());
  EXPECT_EQ(reply.mutable_warble()->text(),request.text());
  EXPECT_EQ(reply.mutable_warble()->id(),"1");
  EXPECT_EQ(reply.mutable_warble()->parent_id(),request.parent_id());
}

// Test: a user warbles a new warble that is not his/her first warble,
//       and this new warble is the reply to a current warble without any replies.
// Expected: 1. Put a new record for this warble: key = "warble_100", value = "It's my second warble.".
//           2. Append this warble id to the user_warbles list: key = "user_warbles_user_Harry Potter", value = "1,100";
//           3. Append this warble id to the warble_thread list: key = "warble_thread_warble_3", value = "100"
TEST_F(WarbleTest, shouldReturnNewWarbleWhenNewWarbleReplyToAnotherWarbleWithoutReplies) {
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

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my second warble.");
  request.set_parent_id("3");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  Payload reply_payload = warble_->WarbleText(mock_payload);

  WarbleReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(reply.mutable_warble()->username(),request.username());
  EXPECT_EQ(reply.mutable_warble()->text(),request.text());
  EXPECT_EQ(reply.mutable_warble()->id(),"100");
  EXPECT_EQ(reply.mutable_warble()->parent_id(),request.parent_id());
}

// Test: a user warbles a new warble that is not his/her first warble,
//       and this new warble is the reply to a current warble with some replies.
// Expected: 1. Put a new record for this warble: key = "warble_100", value = "It's my second warble.".
//           2. Append this warble id to the user_warbles list: key = "user_warbles_user_Harry Potter", value = "1,100";
//           3. Append this warble id to the warble_thread list: key = "warble_thread_warble_3", value = "4,5,6,100"
TEST_F(WarbleTest, shouldReturnNewWarbleWhenNewWarbleReplyToAnotherWarbleWithReplies) {
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

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my second warble.");
  request.set_parent_id("3");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  Payload reply_payload = warble_->WarbleText(mock_payload);

  WarbleReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(reply.mutable_warble()->username(),request.username());
  EXPECT_EQ(reply.mutable_warble()->text(),request.text());
  EXPECT_EQ(reply.mutable_warble()->id(),"100");
  EXPECT_EQ(reply.mutable_warble()->parent_id(),request.parent_id());
}

// Test: ReadThread of a warble with id "123" that has no replies.
// Expected: Expect call KeyValueStore Get function once with the key "warble_thread_warble_123"
//           ReadThread function return the empty vector of strings
TEST_F(WarbleTest, shouldReturnEmptyStringVectorWhenReadThreadWithoutAnyReplies) {
  std::string mock_warble_thread_key = "warble_thread_warble_123";
  StringVector mock_key_vector = {mock_warble_thread_key};
  StringOptionalVector mock_value_vector = {StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
         .Times(1)
         .WillOnce(Return(mock_value_vector));

  ReadRequest request;
  request.set_warble_id("123");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  Payload reply_payload = warble_->ReadThread(mock_payload);

  ReadReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_TRUE(reply.warbles().empty());
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


  std::vector<Warble> mock_warbles;
  StringOptionalVector mock_warbles_as_str;

  for (int i = 3; i < 6; i++) {
    Warble warble;
    timeval time;
    gettimeofday(&time,NULL);
    std::string suffix = std::to_string(i);
    warble.set_username("username" + suffix);
    warble.set_text("I am warble No. " + suffix);
    warble.set_id(suffix);
    warble.set_parent_id("123");
    warble.mutable_timestamp()->set_seconds(time.tv_sec);
    warble.mutable_timestamp()->set_useconds(time.tv_usec);
    mock_warbles.push_back(warble);
    mock_warbles_as_str.push_back(warble.SerializeAsString());
  }

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .Times(1)
      .WillOnce(Return(mock_warbles_as_str));

  ReadRequest request;
  request.set_warble_id("123");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  Payload reply_payload = warble_->ReadThread(mock_payload);

  ReadReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(std::to_string(reply.warbles_size()),
            std::to_string(mock_warbles.size()));

  for (int i = 0; i < reply.warbles_size(); i++) {
    Warble actual_warble = reply.warbles().Get(0);
    Warble expected_warble = mock_warbles.at(0);
    EXPECT_EQ(actual_warble.username(), expected_warble.username());
    EXPECT_EQ(actual_warble.id(), expected_warble.id());
    EXPECT_EQ(actual_warble.text(), expected_warble.text());
    EXPECT_EQ(actual_warble.parent_id(), expected_warble.parent_id());
    EXPECT_EQ(actual_warble.timestamp().seconds(),
              expected_warble.timestamp().seconds());
    EXPECT_EQ(actual_warble.timestamp().useconds(),
              expected_warble.timestamp().useconds());
  }
}
