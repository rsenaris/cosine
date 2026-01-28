#pragma once

#include <string>

#define DECLARE_STR(TYPE)                 \
    std::string   to_string(TYPE);        \
    TYPE        from_string(std::string);


enum class CalibrationBelt {
  STRAIGHT,
  SPIRAL,
  NONE,
};
DECLARE_STR(CalibrationBelt)


#undef TO_FROM_STR
