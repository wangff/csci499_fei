#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "func_platform.h"
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

// Mock Class of Warble
// Used for dependency injection for Func_platform constructor
class MockWarble: public Warble {
 public:
  // TODO
  MockWarble(const StoragePtr &storage_ptr) : Warble(storage_ptr) {}
};

// Init the global variables for all the test cases in this test suite
class FuncPlatformTest: public ::testing::Test {
 public:
  FuncPlatformTest(): mock_store_(new MockStorage), mock_warble_(new MockWarble(mock_store_)) {
    auto platform = new FuncPlatform(mock_store_, mock_warble_);
    service_ = std::unique_ptr<FuncPlatform>(platform);
  }

 protected:
  // func platform, core arch.
  std::unique_ptr<FuncPlatform> service_;

  // dependencies
  StoragePtr mock_store_;
  WarblePtr mock_warble_;
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