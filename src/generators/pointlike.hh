#pragma once

#include "G4ParticleDefinition.hh"
#include "G4PrimaryVertex.hh"
#include "types.hh"

#include "G4ThreeVector.hh"
#include <n4-random.hh>

struct pointlike : public n4::random::direction {
   pointlike(const G4String&, G4ThreeVector, u16, G4double);
  ~pointlike(){}

  G4PrimaryVertex* generate_vertex() const;

private:
  // privatize get method to avoid confusion
  using n4::random::direction::get;

  G4ThreeVector generate_direction   () const { return get(); }
  G4ThreeVector generate_polarization() const { return polarization_.get(); }
  G4double      generate_energy      () const { return energy_; }

  G4ParticleDefinition* particle_;
  G4ThreeVector pos_;
  G4double energy_;
  u16 npart_;
  n4::random::direction polarization_;
};
