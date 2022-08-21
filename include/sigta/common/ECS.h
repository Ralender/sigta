#ifndef SIGTA_COMMOM_ECS_H
#define SIGTA_COMMOM_ECS_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <vector>

#include "sigta/common/Meta.h"
#include "sigta/common/RTTI.h"
#include "sigta/common/RelPtr.h"

namespace sigta {

namespace ecs_detail {

template <typename, typename, typename, typename...> class EntitySpec;

template <typename ECS> class EntityBase {

  template <typename, typename, typename, typename...> friend class EntitySpec;

  typename ECS::entityRTTI ID;

  template <typename Cmp> typename ECS::offsetTy getOffset() {
    return ECS::getOffset(ID, ECS::componentRTTI::template get<Cmp>());
  }

  char *getAddr() { return (char *)this; }

public:
  EntityBase() {}
  template <typename Ty> bool ecs_has() {
    return getOffset<Ty>() != ECS::invalidOffset;
  }
  template <typename Ty> Ty *ecs_get() {
    assert(ecs_has<Ty>());
    return reinterpret_cast<Ty *>(getAddr() + getOffset<Ty>());
  }
  template <typename Ty> Ty *ecs_get_or_null() {
    return ecs_has<Ty>() ? ecs_get<Ty>() : nullptr;
  }
};

template <typename ECS, typename ParentTy, typename ParentBaseTy,
          typename... CmpTys>
class EntitySpec : std::tuple<CmpTys...>,
                   ECS::entityRTTI::template Inherits<ParentTy, ParentBaseTy> {
  using Tuple = std::tuple<CmpTys...>;

  ParentTy *getParent() const {
    return static_cast<ParentTy *>(const_cast<EntitySpec *>(this));
  }

  typename ECS::rootTy *getRoot() const {
    return static_cast<typename ECS::rootTy *>(getParent());
  }

  Tuple *getTuple() const {
    return static_cast<Tuple *>(const_cast<EntitySpec *>(this));
  }

  static auto getEntityID() {
    return ECS::entityRTTI::template get<ParentTy>();
  }

  struct initTable {
    initTable(char *addr, Tuple *t) {
      (([&] {
         using Cmp = CmpTys;
         if constexpr (ecs_has<Cmp>()) {
           ECS::getOffset(getEntityID(),
                          ECS::componentRTTI::template get<Cmp>()) =
               ((char *)&std::get<Cmp>(*t)) - addr;
         }
       }()),
       ...);
    }
  };

  void fillTableOnFirstUse() {
    static initTable init(getRoot()->getAddr(), getTuple());
  }

public:
  using ECSBase = EntitySpec;

  EntitySpec() {
    static_assert(std::is_final_v<ParentTy>,
                  "dont yet support multiple layers of EntitySpec");
    static_assert(std::is_base_of_v<ParentBaseTy, ParentTy>,
                  "ParentBaseTy should be the base of ParentTy");
    fillTableOnFirstUse();
    getRoot()->ID = getEntityID();
  }
  template <typename Ty> static constexpr bool ecs_has() {
    return meta::Layout<CmpTys...>::template has<Ty>();
  }
  template <typename Ty> Ty *ecs_get() {
    static_assert(ecs_has<Ty>(), "component doesn't exist");
    return &std::get<Ty>(*getTuple());
  }
  template <typename Ty> Ty *ecs_get_or_null() {
    if constexpr (ecs_has<Ty>())
      return ecs_get<Ty>();
    else
      return nullptr;
  }
};
}; // namespace ecs_detail

template <typename RootTy, typename OffsetTy = std::uint16_t,
          typename EntityKindTy = std::uint16_t,
          typename AllocatorTy = std::allocator<OffsetTy>>
struct ecs_impl {
  friend class ecs_detail::EntityBase<ecs_impl>;

  template <typename, typename, typename, typename...>
  friend class sigta::ecs_detail::EntitySpec;

  using rootTy = RootTy;
  using offsetTy = OffsetTy;
  using entityRTTI = sigta::rtti::HierarchyID<RootTy>;
  using componentRTTI = sigta::rtti::LinearID<RootTy, uint16_t>;

  static constexpr OffsetTy invalidOffset =
      std::numeric_limits<OffsetTy>::max();

  static inline std::vector<OffsetTy> Table;
  static inline std::size_t lineLength;

  static OffsetTy &getOffset(entityRTTI ent, componentRTTI comp) {
    return Table[ent.getInt() * lineLength + comp.getInt()];
  }

public:
  static void init() {
    if (!Table.empty())
      return;
    entityRTTI::init();
    Table.assign(entityRTTI::maxID().getInt() * componentRTTI::maxID().getInt(),
                 invalidOffset);
    lineLength = entityRTTI::maxID().getInt();
  }

  using EntityBase = ecs_detail::EntityBase<ecs_impl>;
  template <typename ParentTy, typename ParentBaseTy, typename... CmpTys>
  using EntitySpec =
      ecs_detail::EntitySpec<ecs_impl, ParentTy, ParentBaseTy, CmpTys...>;
}; // namespace ecs

#define SIGTA_ECS_USING_ENTITY_SPEC                                            \
  using ECSBase::ecs_has;                                                      \
  using ECSBase::ecs_get;                                                      \
  using ECSBase::ecs_get_or_null

} // namespace sigta

#endif
