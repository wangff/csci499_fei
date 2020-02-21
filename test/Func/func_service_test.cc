#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "func_platform.h"
#include "storage_abstraction.h"
#include "warble_service.h"

using ::testing::Return;

// Mock Class of StorageAbstraction
// Used for dependency injection for Func_platform constructor
class MockStorage : public StorageAbstraction {
 public:
  MOCK_METHOD2(Put, void(const std::string &, const std::string &));
  MOCK_METHOD1(Get, StringOptionalVector(const StringVector&));
  MOCK_METHOD1(Remove, void(const std::string &));
};

// Mock Class of Warble
// Used for dependency injection for Func_platform constructor
class MockWarble : public WarbleServiceAbstraction {
 public:
  MOCK_METHOD1(RegisterUser, bool(const std::string &user_name));
  MOCK_METHOD3(WarbleText, std::string(const std::string &user_name, const std::string &text, const StringOptional &reply_to));
  MOCK_METHOD2(Follow, void(const std::string &user_name, const std::string &to_follow));
  MOCK_METHOD1(ReadThread, StringVector(const std::string &warble_id));
  MOCK_METHOD1(ReadProfile, Profile(const std::string &user_name));
};

// Init the global variables for all the test cases in this test suite
class FuncPlatformTest: public ::testing::Test {
 public:
  FuncPlatformTest(): mock_store_(new MockStorage), mock_warble_(new MockWarble) {
    auto platform = new FuncPlatform(mock_store_, mock_warble_);
    service_ = std::unique_ptr<FuncPlatform>(platform);

    // Initialize hooking configuration
    service_->Hook(1,"registeruser");
    service_->Hook(2,"warble");
    service_->Hook(3,"follow");
    service_->Hook(4,"read");
    service_->Hook(5,"profile");
  }

 protected:
  // func platform, core arch.
  std::unique_ptr<FuncPlatform> service_;

  // dependencies
  std::shared_ptr<MockStorage> mock_store_;
  std::shared_ptr<MockWarble> mock_warble_;
};

// Test: Hook will store the pair of event function in the local hashmap.
// Expected: When we hook with event_type = 1; function_str = "function1",
//           we expect the pair exists in the local hashmap.
TEST_F(FuncPlatformTest, CanHook) {
  int event_type = 1;
  std::string function_str = "register";
  service_->Hook(event_type, function_str);
  EXPECT_EQ(function_str,service_->hook_dict_[event_type]);
}

// Test: Unkook the mapping relationship between event and function.
// Expected: When we unhook with event_type = 1,
//           we expect nothing from the the local hashmap based on the event_type.
TEST_F(FuncPlatformTest, CanUnhook) {
  int event_type = 1;
  service_->Unhook(event_type);
  EXPECT_EQ("",service_->hook_dict_[event_type]);
}

// Test: Execute with event_type = 1
// Expected: warbler_service_ will call RegisterUser function
//           Event function will return a payload with an empty RegisteruserReply
TEST_F(FuncPlatformTest, ExecuteEvent1) {
  int event_type = 1;
  RegisteruserRequest request;
  RegisteruserReply reply;
  request.set_username("Harry Potter");
  Payload payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, RegisterUser("Harry Potter")).Times(1);
  Payload reply_payload = service_->Execute(event_type, payload);
  reply_payload.UnpackTo(&reply);
  EXPECT_EQ(reply.descriptor()->field_count(),0);
}

// Test: Execute with event_type = 2 without reply to parent id.
// Expected: warbler_service_ will call WarbleText function
//           Event function will return a payload with an WarbleReply with the warble that has expected fields value.
TEST_F(FuncPlatformTest, ExecuteEvent2WithoutReply) {
  int event_type = 2;
  WarbleRequest request;
  WarbleReply reply;
  request.set_username("Harry Potter");
  request.set_text("It's my first warble.");
  Payload payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, WarbleText("Harry Potter","It's my first warble.", StringOptional("")))
             .Times(1)
             .WillOnce(Return("1"));
  Payload reply_payload = service_->Execute(event_type, payload);
  reply_payload.UnpackTo(&reply);
  EXPECT_EQ(reply.descriptor()->field_count(),1);
  // WarbleReply message should have one field: warble.
  Warble warble = reply.warble();
  EXPECT_EQ(warble.descriptor()->field_count(),5);
  // Warble message should have fives fields:
  // username; text; id; parent_id; timestamp;
  EXPECT_EQ(warble.username(), "Harry Potter");
  EXPECT_EQ(warble.text(),"It's my first warble.");
  EXPECT_EQ(warble.id(),"1");
  EXPECT_EQ(warble.parent_id(),"");
}

// Test: Execute with event_type = 2 with reply to parent id.
// Expected: warbler_service_ will call WarbleText function
//           Event function will return a payload with an WarbleReply with the warble that has expected fields value.
TEST_F(FuncPlatformTest, ExecuteEvent2WithReply) {
  int event_type = 2;
  WarbleRequest request;
  WarbleReply reply;
  request.set_username("Harry Potter");
  request.set_text("It's my first warble.");
  request.set_parent_id("3");
  Payload payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, WarbleText("Harry Potter","It's my first warble.", StringOptional("3")))
      .Times(1)
      .WillOnce(Return("1"));
  Payload reply_payload = service_->Execute(event_type, payload);
  reply_payload.UnpackTo(&reply);
  EXPECT_EQ(reply.descriptor()->field_count(),1);
  // WarbleReply message should have one field: warble.
  Warble warble = reply.warble();
  EXPECT_EQ(warble.descriptor()->field_count(),5);
  // Warble message should have fives fields:
  // username; text; id; parent_id; timestamp;
  EXPECT_EQ(warble.username(), "Harry Potter");
  EXPECT_EQ(warble.text(),"It's my first warble.");
  EXPECT_EQ(warble.id(),"1");
  EXPECT_EQ(warble.parent_id(),"3");
}

// Test: Execute with event_type = 3
// Expected: warbler_service_ will call Follow function
//           Event function will return a payload with an empty FollowReply
TEST_F(FuncPlatformTest, ExecuteEvent3) {
  int event_type = 3;
  FollowRequest request;
  FollowReply reply;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  Payload payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, Follow("Harry Potter", "Lord Voldmort")).Times(1);
  Payload reply_payload = service_->Execute(event_type, payload);
  reply_payload.UnpackTo(&reply);
  EXPECT_EQ(reply.descriptor()->field_count(),0);
}

// Test: Execute with event_type = 5
// Expected: warbler_service_ will call ReadProfile function
//           Event function will return a payload with an ProfileReply that has expected followings and followers.
TEST_F(FuncPlatformTest, ExecuteEvent5) {
  int event_type = 5;
  ProfileRequest request;
  ProfileReply reply;
  request.set_username("Harry Potter");
  Payload payload;
  payload.PackFrom(request);

  Profile mock_profile;
  mock_profile.profile_followings.push_back("following1");
  mock_profile.profile_followings.push_back("following2");
  mock_profile.profile_followings.push_back("following3");
  mock_profile.profile_followers.push_back("follower1");
  mock_profile.profile_followers.push_back("follower2");
  mock_profile.profile_followers.push_back("follower3");

  EXPECT_CALL(*mock_warble_, ReadProfile("Harry Potter"))
             .Times(1)
             .WillOnce(Return(mock_profile));
  Payload reply_payload = service_->Execute(event_type, payload);
  reply_payload.UnpackTo(&reply);
  EXPECT_EQ(reply.descriptor()->field_count(),2);
  EXPECT_EQ(reply.following_size(),mock_profile.profile_followings.size());
  EXPECT_EQ(reply.followers_size(),mock_profile.profile_followers.size());

  for(int i = 0; i < mock_profile.profile_followings.size(); i++) {
    EXPECT_EQ(reply.following(i), mock_profile.profile_followings.at(i));
  }

  for(int i = 0; i < mock_profile.profile_followers.size(); i++) {
    EXPECT_EQ(reply.followers(i), mock_profile.profile_followers.at(i));
  }
}

// Test: Execute with event_type = 6, which does not exist in hooking configuration.
// Expected: Execute function will return an empty payload.
TEST_F(FuncPlatformTest, ExecuteEventNotExist) {
  int event_type = 6;
  Payload payload;
  Payload reply_payload = service_->Execute(event_type, payload);
  EXPECT_EQ(reply_payload.GetTypeName(), "google.protobuf.Any");
}

// Test: Execute with event_type = 4 and the parameter warble_id which has no replies.
// Expected: warble_service will call ReadThread function.
//           and will return an empty vector of warbles
TEST_F(FuncPlatformTest, shouldReturnEmptyVectorOfWarbleWhenCallExecuteWithEvetType4AndWarbleIdWithoutAnyReplies) {
  int event_type = 4;
  ReadRequest request;
  ReadReply reply;
  request.set_warble_id("1");
  Payload payload;
  payload.PackFrom(request);

  StringVector mock_warbles_as_str;

  EXPECT_CALL(*mock_warble_, ReadThread("1"))
            .Times(1)
            .WillOnce(Return(mock_warbles_as_str));

  Payload reply_payload = service_->Execute(event_type, payload);
  reply_payload.UnpackTo(&reply);
  EXPECT_TRUE(reply.warbles().empty());
}

// Test: Execute with event_type = 4 and the parameter warble_id which has no replies.
// Expected: warble_service will call ReadThread function.
//           and will return an empty vector of warbles
TEST_F(FuncPlatformTest, shouldReturnVectorOfWarbleWhenCallExecuteWithEvetType4AndWarbleIdWithReplies) {
  int event_type = 4;
  ReadRequest request;
  ReadReply reply;
  request.set_warble_id("2");
  Payload payload;
  payload.PackFrom(request);

  std::vector<Warble> mock_warbles;
  StringVector mock_warbles_as_str;

  for (int i = 3; i < 6; i++) {
    Warble warble;
    timeval time;
    gettimeofday(&time,NULL);
    std::string suffix = std::to_string(i);
    warble.set_username("username"+suffix);
    warble.set_text("I my warble No. "+suffix);
    warble.set_id(suffix);
    warble.set_parent_id("2");
    warble.mutable_timestamp()->set_seconds(time.tv_sec);
    warble.mutable_timestamp()->set_useconds(time.tv_usec);
    mock_warbles.push_back(warble);
    mock_warbles_as_str.push_back(warble.SerializeAsString());
  }

  EXPECT_CALL(*mock_warble_, ReadThread("2"))
      .Times(1)
      .WillOnce(Return(mock_warbles_as_str));

  Payload reply_payload = service_->Execute(event_type, payload);
  reply_payload.UnpackTo(&reply);

  EXPECT_EQ(std::to_string(reply.warbles_size()),std::to_string(mock_warbles.size()));

  for (int i = 0; i < reply.warbles_size(); i++) {
    Warble actual_warble = reply.warbles().Get(0);
    Warble expected_warble = mock_warbles.at(0);

    EXPECT_EQ(actual_warble.username(), expected_warble.username());
    EXPECT_EQ(actual_warble.text(), expected_warble.text());
    EXPECT_EQ(actual_warble.id(), expected_warble.id());
    EXPECT_EQ(actual_warble.parent_id(), expected_warble.parent_id());
    EXPECT_EQ(actual_warble.timestamp().seconds(), expected_warble.timestamp().seconds());
    EXPECT_EQ(actual_warble.timestamp().useconds(), expected_warble.timestamp().useconds());
  }
}