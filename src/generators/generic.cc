#include "generators/generic.hh"

#include <n4-inspect.hh>

generic_generator::generic_generator(const G4String& particle_name, u16 nparticles)
  : particle_(n4::find_particle(particle_name))
  , nparticles_(nparticles)
{
}


G4PrimaryVertex* generic_generator::generate_vertex() const {
  auto vertex = new G4PrimaryVertex(generate_position(), 0.);

  for (auto i=0; i<nparticles_; i++) {
    auto dir = generate_direction();
    auto pol = generate_polarization();
    auto ene = generate_energy();
    auto particle = new G4PrimaryParticle(particle_, dir.x(), dir.y(), dir.z(), ene);

    particle -> SetPolarization(pol);
    vertex   -> SetPrimary(particle);
  }
  return vertex;
}
