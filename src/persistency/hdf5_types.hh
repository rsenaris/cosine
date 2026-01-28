#pragma once

#include "core/config.hh"
#include "core/types.hh"

#include <highfive/H5DataType.hpp>

#define DECLARE_HF_TYPE_BUILDER(TYPE_NAME, FUNCTION)   \
  HighFive::CompoundType FUNCTION();                   \
  namespace HighFive {                                 \
    template <> DataType create_datatype<TYPE_NAME>(); \
  }

DECLARE_HF_TYPE_BUILDER(VolumeChange , create_volume_change)
DECLARE_HF_TYPE_BUILDER(SensorHit    , create_sensor_hit)
DECLARE_HF_TYPE_BUILDER(Track        , create_track)
DECLARE_HF_TYPE_BUILDER(IonizationHit, create_ionization_hit)
DECLARE_HF_TYPE_BUILDER(Source       , create_source)
DECLARE_HF_TYPE_BUILDER(ConfPar      , create_config)

#undef DECLARE_HF_TYPE_BUILDER
