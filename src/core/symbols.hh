#pragma once

#include <string>

template <typename T>
T from_string(std::string);

#define DECLARE_STR(TYPE)                       \
    std::string   to_string(TYPE);              \
    template <>                                 \
    TYPE        from_string<TYPE>(std::string);


enum class CalibrationBelt {
  STRAIGHT,
  SPIRAL,
  NONE,
};
DECLARE_STR(CalibrationBelt)


enum class EventGenerator {
  DEBUG,
  S1,
  S2,
  KR,
  FE,
};
DECLARE_STR(EventGenerator)

enum class Medium {
  XENON,
  KRYPTON,
  ARGON,
};
DECLARE_STR(Medium)

#undef TO_FROM_STR
