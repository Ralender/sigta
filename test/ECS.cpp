#include "sigta/common/ECS.h"
#include "TestCommon.h"
#include "gtest/gtest.h"

using namespace sigta;

namespace {

struct TestTopLevelEntity;

using ecs = sigta::ecs_impl<TestTopLevelEntity>;

struct TestComponent {
  bool b;
};
struct TestComponent1 {
  int i;
};
struct TestComponent2 {
  void* p;
};
struct TestComponent12 {
  short s;
};

struct TestTopLevelEntity : ecs::EntityBase {};

struct TestEntity1 final : TestTopLevelEntity,
                           ecs::EntitySpec<TestEntity1, TestTopLevelEntity,
                                           TestComponent1, TestComponent12> {
  SIGTA_ECS_USING_ENTITY_SPEC;
  bool b;
};

struct TestEntity2 final : TestTopLevelEntity,
                           ecs::EntitySpec<TestEntity2, TestTopLevelEntity,
                                           TestComponent2, TestComponent12> {
  SIGTA_ECS_USING_ENTITY_SPEC;
  void *v;
  bool b;
};

struct TestEntity3 final
    : TestTopLevelEntity,
      ecs::EntitySpec<TestEntity3, TestTopLevelEntity, ComplexObj> {
  SIGTA_ECS_USING_ENTITY_SPEC;
  void *v;
  bool b;
};

TEST(ECS, has) {
  ecs::init();
  auto ent1 = std::make_unique<TestEntity1>();
  auto ent2 = std::make_unique<TestEntity2>();
  TestTopLevelEntity* base1 = ent1.get();
  TestTopLevelEntity* base2 = ent2.get();

  static_assert(TestEntity1::ecs_has<TestComponent1>());
  static_assert(TestEntity1::ecs_has<TestComponent12>());
  static_assert(!TestEntity1::ecs_has<TestComponent>());
  static_assert(!TestEntity1::ecs_has<TestComponent2>());

  static_assert(TestEntity2::ecs_has<TestComponent2>());
  static_assert(TestEntity2::ecs_has<TestComponent12>());
  static_assert(!TestEntity2::ecs_has<TestComponent>());
  static_assert(!TestEntity2::ecs_has<TestComponent1>());

  EXPECT_TRUE(ent1->ecs_has<TestComponent1>());
  EXPECT_TRUE(ent1->ecs_has<TestComponent12>());
  EXPECT_FALSE(ent1->ecs_has<TestComponent>());
  EXPECT_FALSE(ent1->ecs_has<TestComponent2>());

  EXPECT_TRUE(ent2->ecs_has<TestComponent2>());
  EXPECT_TRUE(ent2->ecs_has<TestComponent12>());
  EXPECT_FALSE(ent2->ecs_has<TestComponent>());
  EXPECT_FALSE(ent2->ecs_has<TestComponent1>());

  EXPECT_TRUE(base1->ecs_has<TestComponent1>());
  EXPECT_TRUE(base1->ecs_has<TestComponent12>());
  EXPECT_FALSE(base1->ecs_has<TestComponent>());
  EXPECT_FALSE(base1->ecs_has<TestComponent2>());

  EXPECT_TRUE(base2->ecs_has<TestComponent2>());
  EXPECT_TRUE(base2->ecs_has<TestComponent12>());
  EXPECT_FALSE(base2->ecs_has<TestComponent>());
  EXPECT_FALSE(base2->ecs_has<TestComponent1>());
}

TEST(ECS, layout) {
  ecs::init();
  auto ent1 = std::make_unique<TestEntity1>();
  auto ent2 = std::make_unique<TestEntity2>();
  TestTopLevelEntity* base1 = ent1.get();
  TestTopLevelEntity* base2 = ent2.get();

  EXPECT_EQ(ent1->ecs_get<TestComponent1>(), base1->ecs_get<TestComponent1>());
  EXPECT_EQ(ent1->ecs_get<TestComponent12>(), base1->ecs_get<TestComponent12>());
  EXPECT_EQ(ent2->ecs_get<TestComponent2>(), base2->ecs_get<TestComponent2>());
  EXPECT_EQ(ent2->ecs_get<TestComponent12>(), base2->ecs_get<TestComponent12>());

  EXPECT_EQ(ent1->ecs_get<TestComponent1>(), base1->ecs_get<TestComponent1>());
  EXPECT_EQ(ent1->ecs_get<TestComponent12>(), base1->ecs_get<TestComponent12>());
  EXPECT_EQ(ent2->ecs_get<TestComponent2>(), base2->ecs_get<TestComponent2>());
  EXPECT_EQ(ent2->ecs_get<TestComponent12>(), base2->ecs_get<TestComponent12>());
}

TEST(ECS, complexTypes) {
  static_assert(std::is_trivially_destructible<TestEntity1>::value, "");
  static_assert(std::is_trivially_destructible<TestEntity2>::value, "");
  static_assert(!std::is_trivially_destructible<TestEntity3>::value, "");
  static_assert(std::is_trivially_destructible<TestComponent>::value, "");
  static_assert(std::is_trivially_destructible<TestComponent1>::value, "");

  ComplexObj::reset();
  EXPECT_EQ(ComplexObj::constructCount, 0);
  EXPECT_EQ(ComplexObj::destructCount, 0);
  {
    auto ent2 = std::make_unique<TestEntity3>();
    EXPECT_EQ(ComplexObj::constructCount, 1);
    EXPECT_EQ(ComplexObj::destructCount, 0);
  }
  EXPECT_EQ(ComplexObj::constructCount, 1);
  EXPECT_EQ(ComplexObj::destructCount, 1);
}

} // namespace
