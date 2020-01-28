#include "../../src/KeyValueStore/threadsafe_map.h"
#include <string>
#include <gtest/gtest.h>
#include <thread>

TEST(KeyValueStore, DefaultConstructor)
{
  threadsafe_map m;
  auto mock_value = m.get("100");
  EXPECT_EQ(0u, mock_value.size());
}

TEST(KeyValueStore, AddANewPair)
{
  threadsafe_map m;
  std::string key = "100";
  std::string value = "I am a key, value is 100";
  m.put(key, value);
  EXPECT_EQ(value, m.get(key));
}

TEST(KeyValueStore, remove)
{
  threadsafe_map m;
  std::string key = "100";
  std::string value = "I am a key, value is 100";
  m.put(key, value);
  EXPECT_EQ(value, m.get(key));
  m.remove(key);
  EXPECT_EQ(0u, m.get(key).size());
}

void thread_put_pairs(threadsafe_map *m, int i);
void thread_get_pairs(threadsafe_map *m, std::string *result, int i);
void thread_remove_pairs(threadsafe_map *m, int i);

TEST(KeyValueStore, MultithreadsPut)
{
  threadsafe_map m;

  std::thread threads[10];
  // spawn 10 threads:
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(thread_put_pairs, &m, i);
  }
  for (int i = 0; i < 10; ++i) {
    threads[i].join();
  }

  for (int i = 0; i < 10; ++i) {
    const auto key = std::to_string(i);
    const auto value = key;
    EXPECT_EQ(value,m.get(key));
  }
}


TEST(KeyValueStore, MultithreadsPutGet)
{
  threadsafe_map m;

  std::vector<std::thread> threads(20);
  std::vector<std::string> results(10, "initialization");

  // spawn 10 threads:
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(thread_put_pairs, &m, i);
    threads[i+10] = std::thread(thread_get_pairs, &m, &results[i], i);
  }
  for (int i = 0; i < 20; ++i) {
    threads[i].join();
  }
  for (int i = 0; i < 10; ++i) {
    const auto ExpectedValue = std::to_string(i);
    const auto value = results[i];
    EXPECT_TRUE(value == "" || value == ExpectedValue);
  }
}

TEST(KeyValueStore, MultithreadsPutRemove)
{
  threadsafe_map m;

  std::thread threads[20];
  // spawn 10 threads:
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(thread_put_pairs, &m, i);
    threads[i+10] = std::thread(thread_remove_pairs, &m, i);
  }
  for (int i = 0; i < 20; ++i) {
    threads[i].join();
  }
  for (int i = 0; i < 10; ++i) {
    const auto key = std::to_string(i);
    const auto value = m.get(key);
    EXPECT_TRUE(value == "" || value == key);
  }
}

// we need change value of m, so const is not allowed.
void thread_put_pairs(threadsafe_map *m, int i) {
  const auto key = std::to_string(i);
  const auto value = key;
  m->put(key, value);
}

void thread_get_pairs(threadsafe_map *m, std::string *result, int i){
  const auto key = std::to_string(i);
  *result = m->get(key);
}

void thread_remove_pairs(threadsafe_map *m, int i){
  const auto key = std::to_string(i);
  m->remove(key);
}
