#pragma once

#include "types.hh"

#include <highfive/H5DataType.hpp>

#define DECLARE_HF_TYPE_BUILDER(TYPE_NAME, FUNCTION)   \
  HighFive::CompoundType FUNCTION();                   \
  namespace HighFive {                                 \
    template <> DataType create_datatype<TYPE_NAME>(); \
  }

DECLARE_HF_TYPE_BUILDER(StepData, create_step_data)
