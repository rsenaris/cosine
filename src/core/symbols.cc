#include "core/symbols.hh"

#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>

#include <algorithm>

#define   CASE(TYPE, MEMBER) case TYPE::MEMBER: return #MEMBER;
#define     IF(TYPE, MEMBER) if  (s == #MEMBER) return TYPE::MEMBER;
#define EXCEPT(TYPE, MEMBER) \
  G4Exception("[from_string(TYPE)]", "", FatalException, "Invalid value"); \
  return TYPE::MEMBER; // unreachable, but must return something to match function signature


std::string to_string(CalibrationBelt x) {
  switch (x) {
    CASE(CalibrationBelt, STRAIGHT)
    CASE(CalibrationBelt, SPIRAL  )
    CASE(CalibrationBelt, NONE    )
  }
  return "";
}

template <>
CalibrationBelt from_string<CalibrationBelt>(std::string s) {
  // always uppercase
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);

  IF(CalibrationBelt, STRAIGHT)
  IF(CalibrationBelt, SPIRAL  )
  IF(CalibrationBelt, NONE    )

  EXCEPT(CalibrationBelt, NONE)
}


std::string to_string(EventGenerator x) {
  switch (x) {
    CASE(EventGenerator, S1)
    CASE(EventGenerator, S2)
    CASE(EventGenerator, KR)
  }
  return "";
}

template <>
EventGenerator from_string<EventGenerator>(std::string s) {
  // always uppercase
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);

  IF(EventGenerator, S1)
  IF(EventGenerator, S2)
  IF(EventGenerator, KR)

  EXCEPT(EventGenerator, S1)
}

#undef CASE
#undef IF
