//===----------------------------------------------------------------------===//
//
// This file provides an implementation of a relative pointer. It behaving
// mostly like T* but it stores an offset between this and the data it pointes
// too instead of a pointer.
//
// Because of this:
//  - making a copying, calling .get(), or operator* ... is not free. so this
//    should only be used as an in-memory type in a data-structure and not moved
//    around or used for calculations
//  - when a RelPtr is memcpy'ed(or equivalent) with the data it pointes too it
//    will still pointe to the same data.
//
//===----------------------------------------------------------------------===//

#ifndef SIGTA_COMMON_RELPTR_H
#define SIGTA_COMMON_RELPTR_H

#include <cstdint>
#include <cassert>
#include <limits>
#include <type_traits>

namespace sigta {

template <typename IntTy> struct RelPtrTraits {
  static std::ptrdiff_t getOffset(char *self, char *addr) {
    return addr - self;
  }
  static char *getAddr(char *self, std::ptrdiff_t offset) {
    return self + offset;
  }
  static IntTy getNull(char *self) { return std::numeric_limits<IntTy>::max(); }
};

template <typename T, typename IntTy = std::ptrdiff_t,
          typename TraitTy = RelPtrTraits<IntTy>>
class RelPtr {
  static_assert(std::numeric_limits<IntTy>::max() <=
                std::numeric_limits<std::ptrdiff_t>::max());
  static_assert(std::numeric_limits<IntTy>::min() >=
                std::numeric_limits<std::ptrdiff_t>::min());
  IntTy Offset;

  IntTy checkItFits(std::ptrdiff_t off) {
    assert(static_cast<IntTy>(off) == off && "offset doesn't fit");
    return static_cast<IntTy>(off);
  }
  char* getThis() const {
    return reinterpret_cast<char*>(const_cast<RelPtr*>(this));
  }
  void setOffset(T* other) {
    Offset = checkItFits(((other == nullptr)
                              ? TraitTy::getNull(getThis())
                              : TraitTy::getOffset(getThis(), (char*)other)));
    assert(!other || Offset != TraitTy::getNull(getThis()));
  }

public:
  T *get() const {
    return (Offset == TraitTy::getNull(getThis()))
               ? nullptr
               : ((T *)TraitTy::getAddr(getThis(), Offset));
  }
  T &operator*() const {
    assert(get());
    return *get();
  }
  T *operator->() const {  assert(get()); return get(); }
  T &operator[](std::ptrdiff_t idx) const { assert(get()); return get()[idx]; }
  explicit operator bool() const { return get(); }
  bool operator!() const { return !get(); }

  RelPtr() : RelPtr(nullptr) {}
  /// Not explicit because we do not want to force user to use casts
  RelPtr(std::nullptr_t) { setOffset(nullptr); }
  RelPtr(const T *other) { setOffset(other); }
  RelPtr(T *other) { setOffset(other); }
  RelPtr &operator=(T *other) {
    setOffset(other);
    return *this;
  }

  friend T *operator+(RelPtr ptr, std::ptrdiff_t off) {
    return ptr.get() + off;
  }
  friend T *operator-(RelPtr ptr, std::ptrdiff_t off) {
    return ptr.get() - off;
  }

  /// This is an in-memory type it should not be copied or move
  RelPtr(const RelPtr &) = delete;
  RelPtr &operator=(const RelPtr &) = delete;
};

} // namespace sigta

#endif // SIGTA_COMMON_RELPTR_H
