#include "generators/pointlike.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ThreeVector.hh"
#include <n4-inspect.hh>

pointlike::pointlike(const G4String& particle_name, G4ThreeVector pos, u16 nparticles, G4double energy)
  : particle_(n4::find_particle(particle_name))
  , pos_(pos)
  , energy_(energy)
  , npart_(nparticles)
  , polarization_()
{
  max_cos_theta(-0.5);
}

G4PrimaryVertex* pointlike::generate_vertex() const {
  auto vertex = new G4PrimaryVertex(pos_, 0.);
  for (auto i = 0; i < npart_; i++) {
    auto dir = generate_direction();
    auto pol = generate_polarization();
    auto ene = generate_energy();
    auto particle = new G4PrimaryParticle(particle_, dir.x(), dir.y(), dir.z(), ene);

    particle -> SetPolarization(pol);
    vertex   -> SetPrimary(particle);
  }
  return vertex;
}
