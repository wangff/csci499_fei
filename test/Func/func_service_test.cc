#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "func_platform.h"
#include "storage_abstraction.h"

using ::testing::Return;

// Mock Class of StorageAbstraction
// Used for dependency injection for Func_service constructor
class MockStorage: public StorageAbstraction {
 public:
  MOCK_METHOD2(Put, void(const std::string &, const std::string &));
  MOCK_METHOD1(Get, StringOptionalVector(const StringVector&));
  MOCK_METHOD1(Remove, void(const std::string &));
};

// Init the global variables for all the test cases in this test suite
class FuncPlatformTest: public ::testing::Test {
 public:
  FuncPlatformTest(): mock_store_(new MockStorage), service_(new FuncPlatform(mock_store_)) {}
 protected:
  std::shared_ptr<MockStorage> mock_store_;
  std::unique_ptr<FuncPlatform> service_;
 };

// Test: Hook method of FunPlatform will call Put method of Storage.
// Expected: When we request hooking once with eventType = 1; function = "function1",
//           Put should be called once with arguments key = "event_1", values = "function1".
TEST_F(FuncPlatformTest, CanHook) {
  EXPECT_CALL(*mock_store_, Put("event_"+std::to_string(1),"functon1")).Times(1);
  service_->Hook(1,"functon1");
}

// Test: Unkook method of FunPlatform will call Remove method of Storage.
// Expected: When we request unkooking once with eventType = 1,
//           Remove should be called once with arguments key = "event_1".
TEST_F(FuncPlatformTest, CanRemove) {
  EXPECT_CALL(*mock_store_, Remove("event_"+std::to_string(1))).Times(1);
  service_->Unhook(1);
}

// Test: Execute method of FunPlatform will call Get method of Storage.
// Expected: When we request execute once with eventType = 1 and Payload = payload,
//           Get should be called once with arguments keys = expected_keys and return default_return_vector.
TEST_F(FuncPlatformTest,CanExecute) {
  StringVector expected_keys;
  expected_keys.push_back("event_"+std::to_string(1));
  StringOptionalVector default_return_vector;
  default_return_vector.push_back("function1");
  EXPECT_CALL(*mock_store_, Get(expected_keys))
  .Times(1)
  .WillOnce(Return(default_return_vector));
  Payload payload;
  service_->Execute(1,payload);
}