#include "generators/generic.hh"

#include <n4-inspect.hh>

GenericGenerator::GenericGenerator(const G4String& particle_name, u16 nparticles)
  : particle_(n4::find_particle(particle_name))
  , nparticles_(nparticles)
{
}
