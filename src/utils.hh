#pragma once

#include <functional>
#include <utility>

#define BREAKPOINT(LABEL) {                               \
  std::cout << "========================================" \
            << LABEL                                      \
            << "========================================" \
            << std::endl;                                 \
  }


template <class T, typename... Args>
std::function<void(T)> join(std::function<void(T)> f1, std::function<void(T)> f2) {
  return [f1, f2](T x) {
    f1(std::forward<T>(x));
    f2(std::forward<T>(x));
  };
}
