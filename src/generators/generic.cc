#include "generators/generic.hh"
#include "G4ParticleDefinition.hh"

#include <G4IonTable.hh>

#include <n4-inspect.hh>

generic_generator::generic_generator(const G4String& particle_name, u32 nparticles)
  : particle_def_([=] {return n4::find_particle(particle_name);})
  , nparticles_(nparticles)
{}

generic_generator::generic_generator(u32 atomic_number, u32 mass_number,
                                     f32 energy_level, u32 nparticles)
  : particle_def_( [=] { return G4IonTable::GetIonTable() -> GetIon(atomic_number, mass_number, energy_level);})
  , nparticles_(nparticles)
{}

generic_generator::generic_generator(G4ParticleDefinition* pdef, u32 nparticles)
  : particle_def_( [=] { return pdef;})
  , nparticles_(nparticles)
{}


G4PrimaryVertex* generic_generator::generate_vertex() const {
  auto vertex = new G4PrimaryVertex(generate_position(), 0.);

  for (auto i=0u; i<nparticles_; i++) {
    auto dir = generate_direction();
    auto pol = generate_polarization();
    auto ene = generate_energy();

    auto particle = new G4PrimaryParticle(particle_def(), dir.x(), dir.y(), dir.z());
    particle -> SetKineticEnergy(ene);
    particle -> SetPolarization(pol);
    vertex   -> SetPrimary(particle);
  }
  return vertex;
}
