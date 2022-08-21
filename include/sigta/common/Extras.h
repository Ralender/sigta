//===----------------------------------------------------------------------===//
// Provide small shared utilities
//===----------------------------------------------------------------------===//

#ifndef SIGTA_COMMON_EXTRA_H
#define SIGTA_COMMON_EXTRA_H

#include <cstdint>
#include <thread>
#include <cassert>

namespace sigta {
namespace extra{

template <typename T>
void assertSingleThread() {
  static auto id = std::this_thread::get_id();
  assert(id == std::this_thread::get_id());
}

template<typename ParentTy>
struct EquallyComparable {
  const ParentTy* getParent() const { return static_cast<const ParentTy*>(this); }
  bool operator!=(const ParentTy& other) const {
    return !(*getParent() == other);
  }
};

template <typename ParentTy>
struct Comparable : EquallyComparable<ParentTy> {
  const ParentTy* getParent() const { return static_cast<const ParentTy*>(this); }
  bool operator>=(const ParentTy& other) const { return !(*getParent() < other); }
  bool operator<=(const ParentTy& other) const {
    return (*getParent() < other) || (*getParent() == other);
  }
  bool operator>(const ParentTy& other) const {
    return !(*getParent() < other) && !(*getParent() == other);
  }
};

}
}

#endif
