#pragma once

#include "types.hh"

#include <highfive/H5DataType.hpp>

#define DECLARE_HF_TYPE_BUILDER(TYPE_NAME, FUNCTION)   \
  HighFive::CompoundType FUNCTION();                   \
  namespace HighFive {                                 \
    template <> DataType create_datatype<TYPE_NAME>(); \
  }

DECLARE_HF_TYPE_BUILDER(VolumeChange, create_volume_change)
DECLARE_HF_TYPE_BUILDER(SensorHit   , create_sensor_hit)
DECLARE_HF_TYPE_BUILDER(Interaction , create_interaction)


#undef DECLARE_HF_TYPE_BUILDER
