#include "persistence.h"

#include <string>

#include "gtest/gtest.h"

namespace cs499_fei {

// Test: deserialize a file that does not exist
// Expect: return an empty map
TEST(Persistence, ShouldGetEmptyMapWhenDeserializeAFileNotExist) {
  Persistence p;
  std::string mock_file = "data";
  StringKVMap ret_map = p.deserialize(mock_file);
  EXPECT_TRUE(ret_map.empty());
}

// Test: serialize a map to the file and then deserialize the file to the map.
// Expect: return the map with same value as the original one
TEST(KeyValueStore, ShouldGetExpectedMapWhenSerializeAndDeserialize) {
  Persistence p;
  std::string mock_file = "data";
  StringKVMap mock_map;
  mock_map["1 is 1"] = "How are you?";
  mock_map["2 is 2"] = "Fine, # thank you.";
  mock_map["3 is not 3"] = "How do you do # ? What are you doing??";
  mock_map["this is a string contains many chars"] = "\t\n####,,,,";
  p.serialize(mock_map, mock_file);
  StringKVMap ret_map = p.deserialize(mock_file);
  EXPECT_EQ(mock_map.size(), ret_map.size());
  EXPECT_EQ(mock_map["1 is 1"], ret_map["1 is 1"]);
  EXPECT_EQ(mock_map["2 is 2"], ret_map["2 is 2"]);
  EXPECT_EQ(mock_map["3 is not 3"], ret_map["3 is not 3"]);
  EXPECT_EQ(mock_map["this is a string contains many chars"],
            ret_map["this is a string contains many chars"]);
}
}  // namespace cs499_fei
