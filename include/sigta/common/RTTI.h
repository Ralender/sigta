//===----------------------------------------------------------------------===//
// Provide various way to identify and classify types at runtime
//===----------------------------------------------------------------------===//

#ifndef SIGTA_COMMON_RTTI_H
#define SIGTA_COMMON_RTTI_H

#include <atomic>
#include <cassert>
#include <cstdint>

#include "sigta/common/Extras.h"

namespace sigta {
namespace rtti {

/// Generate a Linearly increasing ID for each type
/// expects a UniquerTy to create a category of IDs
/// This enables having mutiple LinearID in the same program
/// That share the same value range
template <typename UniquerTy, typename IDTy = uint16_t, IDTy Start = 0>
class LinearID
    : public extra::EquallyComparable<LinearID<UniquerTy, IDTy, Start>> {
  IDTy ID;

  LinearID(IDTy id) : ID(id) {}

#ifndef NDEBUG
  static inline bool isFrozen{false};
#endif
  static IDTy& internalCount() {
    static IDTy count{Start};
    extra::assertSingleThread<LinearID>();
    return count;
  }

  template <typename Ty>
  struct initT {
    IDTy id;
    initT() {
      assert(!isFrozen && "accessed before initialization was done");
      id = internalCount()++;
    }
  };
  template <typename Ty>
  static inline initT<Ty> init;

public:
  LinearID(const LinearID&) = default;
  static IDTy countIDs() { return {maxID().ID - Start}; }

  static LinearID maxID() {
#ifndef NDEBUG
    isFrozen = true;
#endif
    return {internalCount()};
  }

  template <typename Ty>
  static LinearID get() {
#ifndef NDEBUG
    isFrozen = true;
#endif
    return {init<Ty>.id};
  }

  IDTy getInt() const { return ID; }
  bool operator==(LinearID Other) const { return ID == Other.ID; }
};

/// Generate a gobally unique ID for each type
class UniqueID : public extra::EquallyComparable<UniqueID> {
  void* ID;

  UniqueID(void* p) : ID(p) {}

public:
  template <typename Ty>
  static UniqueID get() {
    static int64_t id;
    return {&id};
  }
  bool operator==(UniqueID Other) const { return ID == Other.ID; }
};

/// Generate IDs suitable to be used to identify members of a Hierarchy
/// expects a BaseTy, it is used to identify the Hierarchy
template <typename BaseTy, typename IDTy = uint16_t, IDTy start = 0>
class HierarchyID
    : public extra::EquallyComparable<HierarchyID<BaseTy, IDTy, start>> {
  IDTy ID;

  HierarchyID(IDTy p) : ID(p) {}

#ifndef NDEBUG
  static inline bool isFrozen{false};
#endif

  union Node {
    struct A {
      IDTy min;
      IDTy max;
    } id;
    struct B {
      Node* child;
      Node* next;
    } graph;
  };

  template <typename Ty>
  static inline Node data;

  /// marked noinline to prevent template bloat
  static __attribute__((noinline)) void buildGraph(Node* self, Node* parent) {
    assert(!isFrozen);
    extra::assertSingleThread<HierarchyID>();
    Node** addr = &parent->graph.child;
    while (*addr)
      addr = &(*addr)->graph.next;

    *addr = self;
  }

  template <typename Ty, typename ParentTy>
  struct initT {
    initT() {
      Node* parent = &data<ParentTy>;
      Node* self = &data<Ty>;
      buildGraph(self, parent);
    }
  };

  template <typename Ty, typename ParentTy>
  static inline initT<Ty, ParentTy> graphBuilder;

  static void recursiveIDBuilder(Node* n, IDTy& id) {
    if (!n)
      return;
    Node* child = n->graph.child;
    n->id.min = id++;
    recursiveIDBuilder(child, id);
    n->id.max = id;
    recursiveIDBuilder(n->graph.next, id);
  }

public:
  HierarchyID() = default;

  /// Must be called inside main before getting any IDs
  static void init() {
    assert(!isFrozen && "already initialized");
    extra::assertSingleThread<HierarchyID>();
#ifndef NDEBUG
    isFrozen = true;
#endif
    Node* root = &data<BaseTy>;
    IDTy id = start;
    recursiveIDBuilder(root, id);
  }

  /// Used to inform the system that Ty is a child of ParentTy
  template <typename Ty, typename ParentTy>
  struct Inherits {
    Inherits() {
      void* p = &graphBuilder<Ty, ParentTy>;
      (void)p;
      assert(p);
    }
  };

  static HierarchyID maxID() { return {data<BaseTy>.id.max}; }

  /// Return an HierarchyID for the Ty
  template <typename Ty>
  static HierarchyID get() {
    assert(isFrozen && "used before it is ready");
    return {data<Ty>.id.min};
  }

  /// Return true if the class identified by id is Ty or one of its subclasses
  template <typename Ty>
  static bool isclassof(HierarchyID id) {
    Node* n = &data<Ty>;
    return id.ID >= n->id.min && id.ID < n->id.max;
  }

  IDTy getInt() const { return ID; }
  bool operator==(HierarchyID other) const { return ID == other.ID; }
};

} // namespace rtti
} // namespace sigta

#endif
