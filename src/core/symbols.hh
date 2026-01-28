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
  S1,
  S2,
  KR,
};
DECLARE_STR(EventGenerator)

#undef TO_FROM_STR
