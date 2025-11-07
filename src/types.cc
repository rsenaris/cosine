#include "types.hh"

#include <cstring>

void copy_str(char *to, const G4String &from, size_t n) {
  memset(to,   0, n);
  std::strcpy(to, from.c_str());
}

StepData make_step_data( const i32&           event
                       , const G4String&      particle_name
                       , const G4String&       pre_volume_name
                       , const G4String&      post_volume_name
                       , const G4ThreeVector&  pre_pos
                       , const G4ThreeVector& post_pos
                       )
{
  StepData data;
  data.event = event;
  copy_str(data.   particle,    particle_name, SHORTSTR);
  copy_str(data. pre_volume,  pre_volume_name, SHORTSTR);
  copy_str(data.post_volume, post_volume_name, SHORTSTR);
  data. pre_x =  pre_pos.x();
  data. pre_y =  pre_pos.y();
  data. pre_z =  pre_pos.z();
  data.post_x = post_pos.x();
  data.post_y = post_pos.y();
  data.post_z = post_pos.z();
  return data;
}

SensorHit make_sensor_hit(const u32& event, const u16& sensor_id, const f32& time) {
  return {event, sensor_id, time};
}
