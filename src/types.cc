#include "types.hh"

#include <cstring>

void copy_str(char* to, const G4String& from) {
  std::strncpy(to, from.c_str(), sizeof(*to));
  to[sizeof(to) - 1] = '\0';
}

StepData::StepData( const G4String&      particle_name
                  , const G4String&       pre_volume_name
                  , const G4String&      post_volume_name
                  , const G4ThreeVector&  pre_pos
                  , const G4ThreeVector& post_pos
                  ) :
 pre_x( pre_pos.x()),
 pre_y( pre_pos.y()),
 pre_z( pre_pos.z()),
post_x(post_pos.x()),
post_y(post_pos.y()),
post_z(post_pos.z())
{
  copy_str(   particle,    particle_name);
  copy_str( pre_volume,  pre_volume_name);
  copy_str(post_volume, post_volume_name);
}
