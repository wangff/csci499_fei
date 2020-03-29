#include "random_generator.h"

#include <iostream>
#include <random>

namespace cs499_fei {
uint32_t randomID() {
  std::random_device rd;
  std::mt19937 engine{rd()};
  std::uniform_int_distribution<uint32_t> distribution(
      0, std::numeric_limits<uint32_t>::max());

  // get random numbers with:
  return distribution(engine);
}
}  // namespace cs499_fei