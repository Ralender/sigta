#include <cstring>
#include "sigta/common/RelPtr.h"
#include "gtest/gtest.h"

using namespace sigta;

namespace {

TEST(RelPtr, Basic) {
  int a = 0;
  RelPtr<int> p1{&a};
  *p1 = 1;
  EXPECT_EQ(a, 1);
  RelPtr<int> p2{&a};
  EXPECT_EQ(p1.get(), p2.get());
  p2[0] = 2;
  EXPECT_EQ(a, 2);
}

TEST(RelPtr, memcpy) {
  struct A {
    RelPtr<int> p;
    int a;
  };
  A a;
  a.a = 1;
  a.p = &a.a;
  A b;
  std::memcpy(&b, &a, sizeof(A));
  EXPECT_EQ(b.a, 1);
  EXPECT_EQ(b.p.get(), &b.a);
  *a.p = 2;
  EXPECT_EQ(a.a, 2);
  *b.p = 2;
  EXPECT_EQ(b.a, 2);
}

TEST(RelPtr, Null) {
  RelPtr<int> p1{nullptr};
  EXPECT_EQ(p1.get(), (int*)nullptr);
  RelPtr<int, unsigned> p2{nullptr};
  EXPECT_EQ(p2.get(), (int*)nullptr);
  struct A {
    RelPtr<int, unsigned> p3;
    int i;
  } a{p2.get(), 0};
  EXPECT_EQ(a.p3.get(), (int*)nullptr);
  a.p3 = &a.i;
  EXPECT_TRUE(a.p3.get() != nullptr);
  EXPECT_EQ(a.p3.get(), &a.i);
  RelPtr<int> p4;
  EXPECT_EQ(p4.get(), (int*)nullptr);
  RelPtr<int, unsigned> p5;
  EXPECT_EQ(p5.get(), (int*)nullptr);
}

} // anonymous namespace
