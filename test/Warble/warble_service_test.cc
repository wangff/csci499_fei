#include "warble_service.h"

#include <sys/time.h>

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "storage_abstraction.h"

using ::testing::Return;

namespace cs499_fei {
// Mock Class of StorageAbstraction
// Used for dependency injection for Warble constructor
class MockStorage : public StorageAbstraction {
 public:
  MOCK_METHOD2(Put, void(const std::string &, const std::string &));
  MOCK_METHOD1(Get, StringOptionalVector(const StringVector &));
  MOCK_METHOD1(Remove, void(const std::string &));
};

// Init the global variables for all the test cases in this test suite
class WarbleTest : public ::testing::Test {
 public:
  WarbleTest() : mock_store_(new MockStorage), warble_(new WarbleService()) {}

 protected:
  // dependencies
  std::shared_ptr<MockStorage> mock_store_;
  std::unique_ptr<WarbleService> warble_;
};

// Test: RegisterUser successfully.
// Expected: Return payload with value.
TEST_F(WarbleTest, shouldReturnPayloadWithValueWhenRegisterUserSuccessfully) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_user_followers_key = "user_followers_user_Harry Potter";
  std::string mock_user_followings_key = "user_followings_user_Harry Potter";

  StringVector mock_key_vector = {mock_user_warbles_key};
  StringOptionalVector mock_user_warbles_value = {StringOptional()};
  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_user_warbles_value));
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "INIT"));
  EXPECT_CALL(*mock_store_, Put(mock_user_followers_key, "INIT"));
  EXPECT_CALL(*mock_store_, Put(mock_user_followings_key, "INIT"));

  Payload mock_payload;
  RegisteruserRequest mock_request;
  mock_request.set_username("Harry Potter");
  mock_payload.PackFrom(mock_request);
  PayloadOptional reply_payload =
      warble_->RegisterUser(mock_payload, mock_store_);
  EXPECT_TRUE(reply_payload.has_value());
}

// Test: RegisterUser unsuccessfully.
// Expected: Return payload without value.
TEST_F(WarbleTest,
       shouldReturnPayloadWithoutValueWhenRegisterUserUnsuccessfully) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_user_followers_key = "user_followers_user_Harry Potter";
  std::string mock_user_followings_key = "user_followings_user_Harry Potter";

  StringVector mock_key_vector = {mock_user_warbles_key};
  StringOptionalVector mock_user_warbles_value = {"INIT"};
  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_user_warbles_value));

  Payload mock_payload;
  RegisteruserRequest mock_request;
  mock_request.set_username("Harry Potter");
  mock_payload.PackFrom(mock_request);
  PayloadOptional reply_payload =
      warble_->RegisterUser(mock_payload, mock_store_);
  EXPECT_FALSE(reply_payload.has_value());
}

// Test: user_name follow to_follow failed since user_name has not been
// registered. Expect: Return the empty PayloadOptional
TEST_F(WarbleTest, shouldReturnEmptyPayloadWhenCallFollowAndUserNameNotExist) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Lord Voldmort";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {StringOptional(),
                                            StringOptional("INIT")};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  Payload mock_payload;
  FollowRequest request;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  mock_payload.PackFrom(request);
  PayloadOptional reply_payload_opt =
      warble_->Follow(mock_payload, mock_store_);
  EXPECT_FALSE(reply_payload_opt.has_value());
}

// Test: user_name follow to_follow failed since to_follow has not been
// registered. Expect: Return the empty PayloadOptional.
TEST_F(WarbleTest, shouldReturnEmptyPayloadWhenCallFollowAndToFollowNotExist) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Lord Voldmort";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {StringOptional("INIT"),
                                            StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  Payload mock_payload;
  FollowRequest request;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  mock_payload.PackFrom(request);
  PayloadOptional reply_payload_opt =
      warble_->Follow(mock_payload, mock_store_);
  EXPECT_FALSE(reply_payload_opt.has_value());
}

// Test: user_name follow to_follow when both user_name and to_follow have empty
// profile. Expected: Return the payload with value.
TEST_F(WarbleTest, shouldPayloadWithValueWhenCallFollowFirstTime) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Lord Voldmort";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {StringOptional("INIT"),
                                            StringOptional("INIT")};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  std::string mock_user_followings = "Lord Voldmort";
  std::string mock_to_follow_followers = "Harry Potter";
  EXPECT_CALL(*mock_store_, Put(user_followings_key, mock_user_followings));
  EXPECT_CALL(*mock_store_,
              Put(to_follow_followers_key, mock_to_follow_followers));

  Payload mock_payload;
  FollowRequest request;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  mock_payload.PackFrom(request);
  PayloadOptional reply_payload = warble_->Follow(mock_payload, mock_store_);
  EXPECT_TRUE(reply_payload.has_value());
}

// Test: user_name follow to_follow when both user_name and to_follow have
// profile Expected: Return the payload with value.
TEST_F(WarbleTest, shouldPayloadWithValueWhenCallFollow) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Lord Voldmort";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {"following1,following2,following3",
                                            "follower1,follower2,follower3"};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  std::string mock_user_followings =
      "following1,following2,following3,Lord Voldmort";
  std::string mock_to_follow_followers =
      "follower1,follower2,follower3,Harry Potter";
  EXPECT_CALL(*mock_store_, Put(user_followings_key, mock_user_followings));
  EXPECT_CALL(*mock_store_,
              Put(to_follow_followers_key, mock_to_follow_followers));

  Payload mock_payload;
  FollowRequest request;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  mock_payload.PackFrom(request);
  PayloadOptional reply_payload = warble_->Follow(mock_payload, mock_store_);
  EXPECT_TRUE(reply_payload.has_value());
}

// Test: Read user's profile when this user does not exist.
// Expected: Return Empty.
TEST_F(
    WarbleTest,
    shouldReturnEmptyProfileWhenReadProfileOfAUserWithoutAnyFollowerOrFollowing) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Harry Potter";
  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};

  StringOptionalVector mock_value_vector = {StringOptional("INIT"),
                                            StringOptional("INIT")};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  Payload mock_payload;
  ProfileRequest request;
  request.set_username("Harry Potter");
  mock_payload.PackFrom(request);
  PayloadOptional reply_payload_opt =
      warble_->ReadProfile(mock_payload, mock_store_);

  ASSERT_TRUE(reply_payload_opt.has_value());
  Payload reply_payload = reply_payload_opt.value();

  ProfileReply reply;
  reply_payload.UnpackTo(&reply);

  auto actual_profile_followings = reply.following();
  auto actual_profile_followers = reply.followers();

  EXPECT_EQ(0, actual_profile_followings.size());
  EXPECT_EQ(0, actual_profile_followers.size());
}

// Test: Read user's profile that is empty.
// Expected: Return the empty PayloadOptional.
TEST_F(WarbleTest, shouldReturnEmptyPayloadWhenReadProfileOfAUserNotExist) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Harry Potter";
  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};

  StringOptionalVector mock_value_vector = {StringOptional(), StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  Payload mock_payload;
  ProfileRequest request;
  request.set_username("Harry Potter");
  mock_payload.PackFrom(request);
  PayloadOptional reply_payload_opt =
      warble_->ReadProfile(mock_payload, mock_store_);

  EXPECT_FALSE(reply_payload_opt.has_value());
}

// Test: Read user's profile that is not empty.
// Expected: The return profile is as expected.
TEST_F(WarbleTest,
       shouldReturnProfileWhenReadProfileOfAUserWithAnyFollowerOrFollowing) {
  std::string user_followings_key = "user_followings_user_Harry Potter";
  std::string to_follow_followers_key = "user_followers_user_Harry Potter";

  StringVector mock_key_vector = {user_followings_key, to_follow_followers_key};
  StringOptionalVector mock_value_vector = {"following1,following2,following3",
                                            "follower1,follower2,follower3"};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
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
  PayloadOptional reply_payload_opt =
      warble_->ReadProfile(mock_payload, mock_store_);

  ASSERT_TRUE(reply_payload_opt.has_value());
  Payload reply_payload = reply_payload_opt.value();

  ProfileReply reply;
  reply_payload.UnpackTo(&reply);

  auto actual_profile_followings = reply.following();
  auto actual_profile_followers = reply.followers();

  EXPECT_EQ(expected_profile.profile_followings.size(),
            actual_profile_followings.size());
  EXPECT_EQ(expected_profile.profile_followers.size(),
            actual_profile_followers.size());

  for (int i = 0; i < expected_profile.profile_followings.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followings.at(i),
              actual_profile_followings.Get(i));
  }

  for (int i = 0; i < expected_profile.profile_followers.size(); i++) {
    EXPECT_EQ(expected_profile.profile_followers.at(i),
              actual_profile_followers.Get(i));
  }
}

// Test: a user warbles a text, but this user does not exist.
// Expected : return an empty PayloadOptional
TEST_F(WarbleTest, shouldReturnEmptyPayloadWhenWarbleWithAUserNotExist) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  StringVector key_vector = {mock_user_warbles_key};

  StringOptionalVector mock_value_vector = {StringOptional()};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .WillOnce(Return(mock_value_vector));

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my first warble.");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->WarbleText(mock_payload, mock_store_);

  EXPECT_FALSE(reply_payload_opt.has_value());
}

// Test: a user warbles a text that replies to another warble, but this reply_to
// warble does not exist. Expected : return an empty PayloadOptional
TEST_F(WarbleTest, shouldReturnEmptyPayloadWhenReplyToNotExist) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_warble_thread_key = "warble_thread_warble_5";
  std::string mock_reply_to_warble_key = "warble_5";
  StringVector key_vector = {mock_user_warbles_key, mock_warble_thread_key,
                             mock_reply_to_warble_key};

  StringOptionalVector mock_value_vector = {"1,2,3", "", ""};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .WillOnce(Return(mock_value_vector));

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my first warble.");
  request.set_parent_id("5");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->WarbleText(mock_payload, mock_store_);

  EXPECT_FALSE(reply_payload_opt.has_value());
}

// Test: a user warbles his/her first warble without reply to any warbles.
// Expected: Return New Warble with the same username, text, id, parent_id with
// the request information.
TEST_F(WarbleTest,
       shouldReturnNewWarbleWhenUserWarbleTheFirstWarbleWithoutReplyTo) {
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  StringVector key_vector = {mock_user_warbles_key};

  StringOptionalVector mock_value_vector = {StringOptional("INIT")};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .WillOnce(Return(mock_value_vector));

  std::string text = "It's my first warble.";

  EXPECT_CALL(*mock_store_, Put("warble_1", testing::_));
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "1"));

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my first warble.");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->WarbleText(mock_payload, mock_store_);

  ASSERT_TRUE(reply_payload_opt.has_value());
  Payload reply_payload = reply_payload_opt.value();

  WarbleReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(reply.mutable_warble()->username(), request.username());
  EXPECT_EQ(reply.mutable_warble()->text(), request.text());
  EXPECT_EQ(reply.mutable_warble()->id(), "1");
  EXPECT_EQ(reply.mutable_warble()->parent_id(), request.parent_id());
}

// Test: a user warbles a new warble that is not his/her first warble,
//       and this new warble is the reply to a current warble without any
//       replies.
// Expected: Return New Warble with the same username, text, id, parent_id with
// the request information.
TEST_F(WarbleTest,
       shouldReturnNewWarbleWhenNewWarbleReplyToAnotherWarbleWithoutReplies) {
  // mock warble id
  warble_->warble_id_ = 100;
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_warble_thread_key = "warble_thread_warble_3";
  std::string mock_warble_key = "warble_3";
  StringVector key_vector = {mock_user_warbles_key, mock_warble_thread_key,
                             mock_warble_key};

  StringOptionalVector mock_value_vector = {"1", StringOptional(),
                                            "It's the No. 3 warble string"};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .WillOnce(Return(mock_value_vector));

  std::string text = "It's my second warble.";

  EXPECT_CALL(*mock_store_, Put("warble_100", testing::_));
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "1,100"));
  EXPECT_CALL(*mock_store_, Put(mock_warble_thread_key, "100"));

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my second warble.");
  request.set_parent_id("3");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->WarbleText(mock_payload, mock_store_);

  ASSERT_TRUE(reply_payload_opt.has_value());
  Payload reply_payload = reply_payload_opt.value();

  WarbleReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(reply.mutable_warble()->username(), request.username());
  EXPECT_EQ(reply.mutable_warble()->text(), request.text());
  EXPECT_EQ(reply.mutable_warble()->id(), "100");
  EXPECT_EQ(reply.mutable_warble()->parent_id(), request.parent_id());
}

// Test: a user warbles a new warble that is not his/her first warble,
//       and this new warble is the reply to a current warble with some replies.
// Expected: Return New Warble with the same username, text, id, parent_id with
// the request information.
TEST_F(WarbleTest,
       shouldReturnNewWarbleWhenNewWarbleReplyToAnotherWarbleWithReplies) {
  // mock warble id
  warble_->warble_id_ = 100;
  std::string mock_user_warbles_key = "user_warbles_user_Harry Potter";
  std::string mock_warble_thread_key = "warble_thread_warble_3";
  std::string mock_warble_key = "warble_3";
  StringVector key_vector = {mock_user_warbles_key, mock_warble_thread_key,
                             mock_warble_key};

  StringOptionalVector mock_value_vector = {"1", "4,5,6",
                                            "It's No.3 warble string"};
  EXPECT_CALL(*mock_store_, Get(key_vector))
      .WillOnce(Return(mock_value_vector));

  std::string text = "It's my second warble.";

  EXPECT_CALL(*mock_store_, Put("warble_100", testing::_));
  EXPECT_CALL(*mock_store_, Put(mock_user_warbles_key, "1,100"));
  EXPECT_CALL(*mock_store_, Put(mock_warble_thread_key, "4,5,6,100"));

  WarbleRequest request;
  request.set_username("Harry Potter");
  request.set_text("It's my second warble.");
  request.set_parent_id("3");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->WarbleText(mock_payload, mock_store_);

  ASSERT_TRUE(reply_payload_opt.has_value());
  Payload reply_payload = reply_payload_opt.value();

  WarbleReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(reply.mutable_warble()->username(), request.username());
  EXPECT_EQ(reply.mutable_warble()->text(), request.text());
  EXPECT_EQ(reply.mutable_warble()->id(), "100");
  EXPECT_EQ(reply.mutable_warble()->parent_id(), request.parent_id());
}

// Test: ReadThread of a warble with id "123" that does not exist.
// Expected: Return empty PayloadOptional.
TEST_F(WarbleTest, shouldReturnEmptyPayloadWhenReadThreadNotExist) {
  std::string mock_warble_thread_key = "warble_thread_warble_123";
  std::string mock_warble_key = "warble_123";
  StringVector mock_key_vector = {mock_warble_thread_key, mock_warble_key};
  StringOptionalVector mock_value_vector = {StringOptional(), StringOptional()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  ReadRequest request;
  request.set_warble_id("123");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->ReadThread(mock_payload, mock_store_);

  EXPECT_FALSE(reply_payload_opt.has_value());
}

// Test: ReadThread of a warble with id "123" that has no replies.
// Expected: ReadThread function return this start warble.
TEST_F(WarbleTest,
       shouldReturnEmptyStringVectorWhenReadThreadWithoutAnyReplies) {
  std::string mock_warble_thread_key = "warble_thread_warble_123";
  std::string mock_warble_key = "warble_123";
  StringVector mock_key_vector = {mock_warble_thread_key, mock_warble_key};

  Warble mock_warble;
  timeval time;
  gettimeofday(&time, NULL);
  mock_warble.set_username("username");
  mock_warble.set_text("I am warble No. 123");
  mock_warble.set_id("123");
  mock_warble.mutable_timestamp()->set_seconds(time.tv_sec);
  mock_warble.mutable_timestamp()->set_useconds(time.tv_usec);

  StringOptionalVector mock_value_vector = {"",
                                            mock_warble.SerializeAsString()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  ReadRequest request;
  request.set_warble_id("123");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->ReadThread(mock_payload, mock_store_);

  ASSERT_TRUE(reply_payload_opt.has_value());
  Payload reply_payload = reply_payload_opt.value();

  ReadReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(reply.warbles_size(), 1);
  Warble actual_warble = reply.warbles(0);
  EXPECT_EQ(actual_warble.username(), mock_warble.username());
  EXPECT_EQ(actual_warble.id(), mock_warble.id());
  EXPECT_EQ(actual_warble.text(), mock_warble.text());
  EXPECT_EQ(actual_warble.parent_id(), mock_warble.parent_id());
  EXPECT_EQ(actual_warble.timestamp().seconds(),
            mock_warble.timestamp().seconds());
  EXPECT_EQ(actual_warble.timestamp().useconds(),
            mock_warble.timestamp().useconds());
}

// Test: ReadThread of a warble with id "123" that has replies.
// Expected: ReadThread function return the vector within three warbles
//           which has the expected username, id, parent_id, time_stamp.
TEST_F(
    WarbleTest,
    shouldReturnTheStringVectorOfRepliesWhenReadThreadOfWarbleHasSomeReplies) {
  std::string mock_warble_thread_key = "warble_thread_warble_123";
  std::string mock_warble_key = "warble_123";
  StringVector mock_key_vector = {mock_warble_thread_key, mock_warble_key};

  Warble mock_start_warble;
  timeval time;
  gettimeofday(&time, NULL);
  mock_start_warble.set_username("username");
  mock_start_warble.set_text("I am warble No. 123");
  mock_start_warble.set_id("123");
  mock_start_warble.mutable_timestamp()->set_seconds(time.tv_sec);
  mock_start_warble.mutable_timestamp()->set_useconds(time.tv_usec);

  StringOptionalVector mock_value_vector = {
      "1,2,3", mock_start_warble.SerializeAsString()};

  EXPECT_CALL(*mock_store_, Get(mock_key_vector))
      .WillOnce(Return(mock_value_vector));

  mock_key_vector.clear();
  mock_key_vector.push_back("warble_1");
  mock_key_vector.push_back("warble_2");
  mock_key_vector.push_back("warble_3");

  std::vector<Warble> mock_warbles;
  mock_warbles.push_back(mock_start_warble);

  StringOptionalVector mock_warbles_as_str;

  for (int i = 3; i < 6; i++) {
    Warble warble;
    timeval time;
    gettimeofday(&time, NULL);
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
      .WillOnce(Return(mock_warbles_as_str));

  ReadRequest request;
  request.set_warble_id("123");

  Payload mock_payload;
  mock_payload.PackFrom(request);

  PayloadOptional reply_payload_opt =
      warble_->ReadThread(mock_payload, mock_store_);

  ASSERT_TRUE(reply_payload_opt.has_value());
  Payload reply_payload = reply_payload_opt.value();

  ReadReply reply;
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(std::to_string(reply.warbles_size()),
            std::to_string(mock_warbles.size()));

  for (int i = 0; i < mock_warbles.size(); i++) {
    Warble actual_warble = reply.warbles().Get(i);
    Warble expected_warble = mock_warbles.at(i);
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
}  // namespace cs499_fei