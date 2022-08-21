#include "sigta/common/RTTI.h"
#include "gtest/gtest.h"

using namespace sigta;

namespace {

TEST(RTTI, UniqueID) {
  std::array arr = {
      rtti::UniqueID::get<struct A>(), rtti::UniqueID::get<struct B>(),
      rtti::UniqueID::get<struct C>(), rtti::UniqueID::get<struct D>(),
      rtti::UniqueID::get<struct E>()};
  for (unsigned i = 0; i < arr.size(); i++)
    for (unsigned k = 0; k < i; k++) {
      EXPECT_NE(arr[i], arr[k]);
      EXPECT_FALSE(arr[i] == arr[k]);
    }
}

TEST(RTTI, LinearID) {
  using TestID = rtti::LinearID<struct OrderedIDTest, unsigned, 2>;
  std::array arr = {
      TestID::get<struct A>(), TestID::get<struct B>(),
      TestID::get<struct C>(), TestID::get<struct D>(),
      TestID::get<struct E>()};
  EXPECT_EQ(5u, TestID::countIDs());
  for (unsigned i = 0; i < arr.size(); i++) {
    for (unsigned k = 0; k < i; k++) {
      EXPECT_NE(arr[i], arr[k]);
      EXPECT_FALSE(arr[i] == arr[k]);
    }
  }
}

struct Base;

using ClassID = rtti::HierarchyID<Base>;

struct Base {
  ClassID ID;
  Base(ClassID id) : ID(id) {}
};

struct A : Base, ClassID::Inherits<A, Base> { A() : Base(ClassID::get<A>()) {}};
struct B : Base, ClassID::Inherits<B, Base> { B() : Base(ClassID::get<B>()) {}};

struct AA : Base, ClassID::Inherits<AA, A> { AA() : Base(ClassID::get<AA>()) {}};
struct AB : Base, ClassID::Inherits<AB, A> { AB() : Base(ClassID::get<AB>()) {}};
struct AC : Base, ClassID::Inherits<AC, A> { AC() : Base(ClassID::get<AC>()) {}};
struct AD : Base, ClassID::Inherits<AD, A> { AD() : Base(ClassID::get<AD>()) {}};
struct AE : Base, ClassID::Inherits<AE, A> { AE() : Base(ClassID::get<AE>()) {}};

struct BA : Base, ClassID::Inherits<BA, B> { BA() : Base(ClassID::get<BA>()) {}};
struct BB : Base, ClassID::Inherits<BB, B> { BB() : Base(ClassID::get<BB>()) {}};
struct BC : Base, ClassID::Inherits<BC, B> { BC() : Base(ClassID::get<BC>()) {}};

struct ACA : Base, ClassID::Inherits<ACA, AC> { ACA() : Base(ClassID::get<ACA>()) {}};
struct ACB : Base, ClassID::Inherits<ACB, AC> { ACB() : Base(ClassID::get<ACB>()) {}};
struct ACC : Base, ClassID::Inherits<ACC, AC> { ACC() : Base(ClassID::get<ACC>()) {}};

TEST(RTTI, HierarchyID) {
  ClassID::init();

  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<A>()));
  EXPECT_TRUE(ClassID::isclassof<B>(ClassID::get<B>()));
  EXPECT_TRUE(ClassID::isclassof<AA>(ClassID::get<AA>()));
  EXPECT_TRUE(ClassID::isclassof<AB>(ClassID::get<AB>()));
  EXPECT_TRUE(ClassID::isclassof<AC>(ClassID::get<AC>()));
  EXPECT_TRUE(ClassID::isclassof<AD>(ClassID::get<AD>()));
  EXPECT_TRUE(ClassID::isclassof<AE>(ClassID::get<AE>()));
  EXPECT_TRUE(ClassID::isclassof<BA>(ClassID::get<BA>()));
  EXPECT_TRUE(ClassID::isclassof<BB>(ClassID::get<BB>()));
  EXPECT_TRUE(ClassID::isclassof<BC>(ClassID::get<BC>()));
  EXPECT_TRUE(ClassID::isclassof<ACA>(ClassID::get<ACA>()));
  EXPECT_TRUE(ClassID::isclassof<ACB>(ClassID::get<ACB>()));
  EXPECT_TRUE(ClassID::isclassof<ACC>(ClassID::get<ACC>()));

  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<A>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<B>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<AA>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<AB>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<AC>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<AD>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<AE>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<BA>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<BB>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<BC>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<ACA>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<ACB>()));
  EXPECT_TRUE(ClassID::isclassof<Base>(ClassID::get<ACC>()));

  EXPECT_EQ(ClassID::get<A>(), ClassID::get<A>());
  EXPECT_EQ(ClassID::get<B>(), ClassID::get<B>());
  EXPECT_EQ(ClassID::get<AA>(), ClassID::get<AA>());
  EXPECT_EQ(ClassID::get<AB>(), ClassID::get<AB>());
  EXPECT_EQ(ClassID::get<AC>(), ClassID::get<AC>());
  EXPECT_EQ(ClassID::get<AD>(), ClassID::get<AD>());
  EXPECT_EQ(ClassID::get<AE>(), ClassID::get<AE>());
  EXPECT_EQ(ClassID::get<BA>(), ClassID::get<BA>());
  EXPECT_EQ(ClassID::get<BB>(), ClassID::get<BB>());
  EXPECT_EQ(ClassID::get<BC>(), ClassID::get<BC>());
  EXPECT_EQ(ClassID::get<ACA>(), ClassID::get<ACA>());
  EXPECT_EQ(ClassID::get<ACB>(), ClassID::get<ACB>());
  EXPECT_EQ(ClassID::get<ACC>(), ClassID::get<ACC>());

  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<AA>()));
  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<AB>()));
  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<AC>()));
  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<AD>()));
  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<AE>()));
  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<AA>()));
  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<AB>()));
  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<AC>()));
  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<AD>()));
  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<AE>()));

  EXPECT_TRUE(ClassID::isclassof<B>(ClassID::get<BA>()));
  EXPECT_TRUE(ClassID::isclassof<B>(ClassID::get<BB>()));
  EXPECT_TRUE(ClassID::isclassof<B>(ClassID::get<BC>()));
  EXPECT_FALSE(ClassID::isclassof<A>(ClassID::get<BA>()));
  EXPECT_FALSE(ClassID::isclassof<A>(ClassID::get<BB>()));
  EXPECT_FALSE(ClassID::isclassof<A>(ClassID::get<BC>()));

  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<ACA>()));
  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<ACB>()));
  EXPECT_TRUE(ClassID::isclassof<A>(ClassID::get<ACC>()));
  EXPECT_TRUE(ClassID::isclassof<AC>(ClassID::get<ACA>()));
  EXPECT_TRUE(ClassID::isclassof<AC>(ClassID::get<ACB>()));
  EXPECT_TRUE(ClassID::isclassof<AC>(ClassID::get<ACC>()));
  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<ACA>()));
  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<ACB>()));
  EXPECT_FALSE(ClassID::isclassof<B>(ClassID::get<ACC>()));
}

} // namespace
