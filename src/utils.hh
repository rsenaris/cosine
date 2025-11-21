#pragma once

#include "types.hh"

#include <functional>
#include <utility>

#define BREAKPOINT(LABEL) {                               \
  std::cout << "========================================" \
            << LABEL                                      \
            << "========================================" \
            << std::endl;                                 \
  }


template<class T>
T intpow(T base, u16 exp) {
  T out = base;
  while (--exp > 0) out *= base;
  return out;
}

template <typename T1, typename T2>
const auto zip(T1&& it1, T2&& it2) {
  using I1 = decltype(std::begin(std::declval<T1>()));
  using I2 = decltype(std::begin(std::declval<T2>()));

  struct zip_iter {
    I1 i1;
    I2 i2;

    bool operator != (const zip_iter& other) const { return this->i1 != other.i1; }
    void operator ++ ()       { ++i1; ++i2; }
    auto operator *  () const { return std::tie(*i1, *i2); }
  };

  struct zip_wrapper {
    T1 v1;
    T2 v2;
    auto begin() const { return zip_iter{ std::begin(v1), std::begin(v2)}; }
    auto end  () const { return zip_iter{ std::end  (v1), std::end  (v2)}; }
  };
  return zip_wrapper{ std::forward<T1>(it1), std::forward<T2>(it2) };
}

template <class T, typename... Args>
std::function<void(T)> join(std::function<void(T)> f1, std::function<void(T)> f2) {
  return [f1, f2](T x) {
    f1(std::forward<T>(x));
    f2(std::forward<T>(x));
  };
}
