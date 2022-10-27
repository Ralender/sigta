
#ifndef SIGTA_TEST_COMMON_H
#define SIGTA_TEST_COMMON_H

#include <atomic>
#include <cstdio>

namespace sigta {

constexpr int thread_count = 10;

struct ComplexObj {
  static inline std::atomic<uint32_t> constructCount = 0;
  static inline std::atomic<uint32_t> destructCount = 0;
  static void reset() {
    constructCount = 0;
    destructCount = 0;
  }
  ComplexObj() { constructCount++; }
  ~ComplexObj() { destructCount++; }
};

} // namespace sigta

#endif
