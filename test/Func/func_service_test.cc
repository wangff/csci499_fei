#include <string>

#include "func_platform.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "storage_abstraction.h"
#include "warble_service.h"

using ::testing::_;
using ::testing::Return;

// Mock Class of StorageAbstraction
// Used for dependency injection for Func_platform constructor
class MockStorage : public StorageAbstraction {
 public:
  MOCK_METHOD2(Put, void(const std::string &, const std::string &));
  MOCK_METHOD1(Get, StringOptionalVector(const StringVector &));
  MOCK_METHOD1(Remove, void(const std::string &));
};

// Mock Class of Warble
// Used for dependency injection for Func_platform constructor
class MockWarble : public WarbleServiceAbstraction {
 public:
  MOCK_METHOD1(RegisterUser, PayloadOptional(const Payload &payload));
  MOCK_METHOD1(WarbleText, PayloadOptional(const Payload &payload));
  MOCK_METHOD1(Follow, PayloadOptional(const Payload &payload));
  MOCK_METHOD1(ReadThread, PayloadOptional(const Payload &payload));
  MOCK_METHOD1(ReadProfile, PayloadOptional(const Payload &payload));
};

// Init the global variables for all the test cases in this test suite
class FuncPlatformTest : public ::testing::Test {
 public:
  FuncPlatformTest()
      : mock_store_(new MockStorage), mock_warble_(new MockWarble) {
    auto platform = new FuncPlatform(mock_store_, mock_warble_);
    service_ = std::unique_ptr<FuncPlatform>(platform);

    // Initialize hooking configuration
    service_->Hook(1, "register");
    service_->Hook(2, "warble");
    service_->Hook(3, "follow");
    service_->Hook(4, "read");
    service_->Hook(5, "profile");
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
TEST_F(FuncPlatformTest, shouldHaveHookConfignAfterhookEventAndFunction) {
  int event_type = 1;
  std::string function_str = "register";
  service_->Hook(event_type, function_str);
  EXPECT_EQ(function_str, service_->hook_dict_[event_type]);
}

// Test: Unkook the mapping relationship between event and function.
// Expected: When we unhook with event_type = 1,
//           we expect nothing from the the local hashmap based on the
//           event_type.
TEST_F(FuncPlatformTest, shouldNotHaveHookConfigAfterUnhookEvent) {
  int event_type = 1;
  service_->Unhook(event_type);
  EXPECT_EQ("", service_->hook_dict_[event_type]);
}

// Test: Execute with event_type = 1
// Expected: warbler_service_ will call RegisterUser function.
TEST_F(FuncPlatformTest, shouldCallRegisteruserWhenExectueEvent1) {
  int event_type = 1;
  RegisteruserRequest request;
  RegisteruserReply reply;

  request.set_username("Harry Potter");
  Payload payload, reply_payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, RegisterUser(_))
      .Times(1)
      .WillOnce(Return(reply_payload));
  PayloadOptional reply_payload_opt = service_->Execute(event_type, payload);
  EXPECT_TRUE(reply_payload_opt.has_value());
}

// Test: Execute with event_type = 2 .
// Expected: warbler_service_ will call WarbleText function.
TEST_F(FuncPlatformTest, shouldCallWarbleTextWhenExectueEvent2) {
  int event_type = 2;
  WarbleRequest request;
  WarbleReply reply;
  request.set_username("Harry Potter");
  request.set_text("It's my first warble.");
  request.set_parent_id("3");
  Payload payload, reply_payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, WarbleText(_))
      .Times(1)
      .WillOnce(Return(reply_payload));
  PayloadOptional reply_payload_opt = service_->Execute(event_type, payload);
  ;
  EXPECT_TRUE(reply_payload_opt.has_value());
}

// Test: Execute with event_type = 3
// Expected: warbler_service_ will call Follow function
TEST_F(FuncPlatformTest, shouldCallFollowWhenExectueEvent3) {
  int event_type = 3;
  FollowRequest request;
  FollowReply reply;
  request.set_username("Harry Potter");
  request.set_to_follow("Lord Voldmort");
  Payload payload, reply_payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, Follow(_))
      .Times(1)
      .WillOnce(Return(reply_payload));
  PayloadOptional reply_payload_opt = service_->Execute(event_type, payload);
  EXPECT_TRUE(reply_payload_opt.has_value());
}

// Test: Execute with event_type = 4 and the parameter warble_id which has no
// replies. Expected: warble_service will call ReadThread function.
TEST_F(FuncPlatformTest, shouldCallReadThreadWhenExectueEvent4) {
  int event_type = 4;
  ReadRequest request;
  ReadReply reply;
  request.set_warble_id("2");
  Payload payload, reply_payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, ReadThread(_))
      .Times(1)
      .WillOnce(Return(reply_payload));
  PayloadOptional reply_payload_opt = service_->Execute(event_type, payload);
  EXPECT_TRUE(reply_payload_opt.has_value());
}

// Test: Execute with event_type = 5
// Expected: warbler_service_ will call ReadProfile function
TEST_F(FuncPlatformTest, shouldCallReadProfileWhenExectueEvent5) {
  int event_type = 5;
  ProfileRequest request;
  ProfileReply reply;
  request.set_username("Harry Potter");
  Payload payload, reply_payload;
  payload.PackFrom(request);

  EXPECT_CALL(*mock_warble_, ReadProfile(_))
      .Times(1)
      .WillOnce(Return(reply_payload));
  PayloadOptional reply_payload_opt = service_->Execute(event_type, payload);
  EXPECT_TRUE(reply_payload_opt.has_value());
}

// Test: Execute with event_type = 6, which does not exist in hooking
// configuration. Expected: Execute function will return an empty payload.
TEST_F(FuncPlatformTest, shouldReturnEmptyPayloadWhenExecuteNotExist) {
  int event_type = 6;
  Payload payload;
  PayloadOptional reply_payload_opt = service_->Execute(event_type, payload);
  EXPECT_FALSE(reply_payload_opt.has_value());
}
