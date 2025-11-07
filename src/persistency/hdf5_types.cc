#include "persistency/hdf5_types.hh"
#include "types.hh"

#define H(TYPE) HighFive::AtomicType<TYPE>{}

// Sometime between HighFive 2.2 and 2.7 the `inline` was added to the
// definition of HIGHFIVE_REGISTER_TYPE. This results in the linker not finding
// the implementations of the functions registered by the macro. So we hack
// around it by defining our own version of the macro without the `inline`.
#define REGISTER_HF_TYPE(type, function) \
  template <> /*inline*/                                 \
  HighFive::DataType HighFive::create_datatype<type>() { \
    return function();                                   \
  }



HighFive::CompoundType create_step_data() {
  return {
    {      "event", H(i32)     },
    {   "particle", H(shortstr)},
    { "pre_volume", H(shortstr)},
    {"post_volume", H(shortstr)},
    {      "pre_x", H(f32)     },
    {      "pre_y", H(f32)     },
    {      "pre_z", H(f32)     },
    {     "post_x", H(f32)     },
    {     "post_y", H(f32)     },
    {     "post_z", H(f32)     },
  };
}
REGISTER_HF_TYPE(StepData, create_step_data)

HighFive::CompoundType create_sensor_hit() {
  return {
    {      "event", H(u32)},
    {  "sensor_id", H(u16)},
    {       "time", H(f32)},
  };
}
REGISTER_HF_TYPE(SensorHit, create_sensor_hit)


#undef REGISTER_HF_TYPE
