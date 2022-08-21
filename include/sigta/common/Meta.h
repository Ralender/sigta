#ifndef SIGTA_COMMOM_META_H
#define SIGTA_COMMOM_META_H

#include <algorithm>
#include <cassert>

namespace sigta {
namespace meta {

template <template <typename> typename Pred>
struct un {
  template <typename T>
  struct neg {
    static constexpr bool value = !Pred<T>::value;
  };
};

template <template <typename> typename Pred1,
          template <typename> typename Pred2>
struct bin {
  template <typename T>
  struct either {
    static constexpr bool value = Pred1<T>::value || Pred2<T>::value;
  };
  template <typename T>
  struct both {
    static constexpr bool value = Pred1<T>::value && Pred2<T>::value;
  };
};

template <typename T1>
struct X {
  template <typename T2>
  struct is_base_of : std::is_base_of<T1, T2> {};
};

template <typename First, typename... Tys>
struct get_first {
  using type = First;
};

template <template <typename> typename, typename...>
struct for_all {
  static constexpr bool value = true;
};

template <template <typename> typename Pred, typename First, typename... Others>
struct for_all<Pred, First, Others...> {
  static constexpr bool value =
      Pred<First>::value && for_all<Pred, Others...>::value;
};

template <template <typename> typename, typename...>
struct assert_for_all {
  static constexpr bool value = true;
};

template <template <typename> typename Pred, typename First, typename... Others>
struct assert_for_all<Pred, First, Others...>
    : assert_for_all<Pred, Others...> {
  static_assert(Pred<First>::value, "failed on this element");
};

template <template <typename> typename, typename...>
struct for_any {
  static constexpr bool value = false;
};

template <template <typename> typename Pred, typename First, typename... Others>
struct for_any<Pred, First, Others...> {
  static constexpr bool value =
      Pred<First>::value || for_any<Pred, Others...>::value;
};

template <template <typename> typename Pred, typename... Tys>
struct for_none {
  static constexpr bool value = !for_any<Pred, Tys...>::value;
};

template <typename T>
constexpr T align_up(T value, T align) {
  return ((value + (align - 1)) / align) * align;
}

template <typename...>
struct Layout {};

template <typename FirstComp, typename... CompTys>
struct Layout<FirstComp, CompTys...> {
  static constexpr std::size_t getSize(std::size_t counter = 0) {
    return Layout<CompTys...>::getSize(
        align_up(counter + sizeof(FirstComp), alignof(FirstComp)));
  }
  static constexpr std::size_t getAlign() {
    return std::max(alignof(FirstComp), Layout<CompTys...>::getAlign());
  }
  template <typename CompTy>
  static constexpr bool has() {
    return std::is_same_v<FirstComp, CompTy> ||
           Layout<CompTys...>::template has<CompTy>();
  }
  template <typename CompTy>
  static constexpr std::size_t getOffset(std::size_t counter = 0) {
    if constexpr (std::is_same_v<FirstComp, CompTy>)
      return align_up(counter, alignof(FirstComp));
    else
      return Layout<CompTys...>::template getOffset<CompTy>(
          align_up(counter + sizeof(FirstComp), alignof(FirstComp)));
  }
};

template <>
struct Layout<> {
  static constexpr std::size_t getSize(std::size_t counter = 0) {
    return std::max<std::size_t>(counter, 1);
  }
  static constexpr std::size_t getAlign() { return 1; }
  template <typename CompTy>
  static constexpr bool has() {
    return false;
  }
  template <typename CompTy>
  static constexpr std::size_t getOffset(std::size_t counter = 0) {
    assert(false && "cant ecs_get an offset for a component that doesnt exist");
    return 0;
  }
};

}
}

#endif
