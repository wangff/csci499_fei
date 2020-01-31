#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "../../src/KeyValueStore/threadsafe_map.h"

using OptionalVector = std::vector<std::optional<std::string>>;

void thread_put_pairs(ThreadsafeMap &m, int i) {
  const auto key = std::to_string(i);
  const auto value = key;
  m.Put(key, value);
}

void thread_get_pairs(const ThreadsafeMap &m, OptionalVector *vec, int i) {
  const auto key = std::to_string(i);
  (*vec)[i] = m.Get(key);
}

void thread_remove_pairs(ThreadsafeMap &m, int i) {
  const auto key = std::to_string(i);
  m.Remove(key);
}

// Test: construct an empty map
// Expected: null when get some key
TEST(KeyValueStore, DefaultConstructor) {
  ThreadsafeMap m;
  auto mock_value = m.Get("100");
  EXPECT_EQ(std::nullopt, mock_value);
}

// Test: Test Get method works well after putting some key-value pair into the map.
// Expected: value "value is 100" given key "100"
TEST(KeyValueStore, AddANewPair) {
  ThreadsafeMap m;
  auto key = "100";
  auto value = "value is 100";
  m.Put(key, value);
  auto ret = m.Get(key);
  EXPECT_EQ(true, ret.has_value());
  EXPECT_EQ(value, ret.value());
}

// Test: Remove method
// Expected: null given a key that has been removed
TEST(KeyValueStore, remove) {
  ThreadsafeMap m;
  std::string key = "100";
  std::string value = "I am a key, value is 100";

  m.Put(key, value);
  EXPECT_EQ(value, m.Get(key).value());

  m.Remove(key);
  EXPECT_EQ(std::nullopt, m.Get(key));
}

// Test: multiple calls of Put method
// Expected: All pairs are put successfully
TEST(KeyValueStore, MultithreadsPut) {
  ThreadsafeMap m;

  std::thread threads[10];

  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(thread_put_pairs, std::ref(m), i);
  }
  for (int i = 0; i < 10; ++i) {
    threads[i].join();
  }

  for (int i = 0; i < 10; ++i) {
    const auto key = std::to_string(i);
    const auto value = key;
    EXPECT_EQ(value, m.Get(key));
  }
}

// Test: multiple calls of Put method and get method
// Expected: during the running, get
//           either expected value based on key or null
TEST(KeyValueStore, MultithreadsPutGet) {
  ThreadsafeMap m;

  std::vector<std::thread> threads(20);
  OptionalVector results(10);

  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(thread_put_pairs, std::ref(m), i);
    threads[i + 10] = std::thread(thread_get_pairs, std::ref(m), &results, i);
  }
  for (int i = 0; i < 20; ++i) {
    threads[i].join();
  }
  for (int i = 0; i < 10; ++i) {
    const auto expected_value = std::to_string(i);
    const auto ret = results[i];

    // doesn't exist or expected target value.
    EXPECT_TRUE(ret == std::nullopt || ret.value() == expected_value);
  }
}

// Test: multiple calls of Put method and remove method
// Expected: after all calls succeed,
//           get either expected value based on key or null
TEST(KeyValueStore, MultithreadsPutRemove) {
  ThreadsafeMap m;

  std::thread threads[20];

  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(thread_put_pairs, std::ref(m), i);
    threads[i + 10] = std::thread(thread_remove_pairs, std::ref(m), i);
  }
  for (int i = 0; i < 20; ++i) {
    threads[i].join();
  }
  for (int i = 0; i < 10; ++i) {
    const auto key = std::to_string(i);
    const auto value = m.Get(key);
    EXPECT_TRUE(value == std::nullopt || value.value() == key);
  }
}

