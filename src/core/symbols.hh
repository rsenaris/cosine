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


#undef TO_FROM_STR
