#include "sigta/common/ManagedObjs.h"
#include "gtest/gtest.h"
#include "TestCommon.h"
#include <thread>
#include <array>

using namespace sigta;

namespace {

ManagedGlobal<ComplexObj> test;
struct A : GlobalRefCount<&test> {
  A() { get(); }
  ~A() { get(); }
};

TEST(ManagedObjs, Basic) {
  ComplexObj::reset();
  {
    A a;
    EXPECT_EQ(1, ComplexObj::constructCount);
    EXPECT_EQ(0, ComplexObj::destructCount);
    A b;
    EXPECT_EQ(1, ComplexObj::constructCount);
    EXPECT_EQ(0, ComplexObj::destructCount);
  }
  EXPECT_EQ(1, ComplexObj::constructCount);
  EXPECT_EQ(1, ComplexObj::destructCount);
  {
    A a;
    EXPECT_EQ(2, ComplexObj::constructCount);
    EXPECT_EQ(1, ComplexObj::destructCount);
    {
      A b;
      EXPECT_EQ(2, ComplexObj::constructCount);
      EXPECT_EQ(1, ComplexObj::destructCount);
    }
    EXPECT_EQ(2, ComplexObj::constructCount);
    EXPECT_EQ(1, ComplexObj::destructCount);
  }
  EXPECT_EQ(2, ComplexObj::constructCount);
  EXPECT_EQ(2, ComplexObj::destructCount);
}

TEST(ManagedObjs, Threads) {
  ComplexObj::reset();
  std::atomic<bool> start = false;
  std::atomic<bool> stop = false;

  auto run = [&] {
    while (!start);
    while (!stop) {
      A a;
      A b;
    }
  };

  std::array<std::thread, thread_count> others;
  for (auto &t : others)
    t = std::thread(run);
  start = true;

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  stop = true;
  for (auto &t : others)
    t.join();
  EXPECT_EQ(ComplexObj::constructCount, ComplexObj::destructCount);
}

}
