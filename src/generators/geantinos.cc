#include "generators/geantinos.hh"

#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>
#include <Randomize.hh>

#include <n4-inspect.hh>

std::function<void(G4Event*)> geantinos(G4ThreeVector pos, G4ThreeVector dir) {
  auto geantino = n4::find_particle("geantino");
  return [pos, dir, geantino](G4Event* event) {
    auto vertex   = new G4PrimaryVertex(pos, 0.);
    auto particle = new G4PrimaryParticle(geantino, dir.x(), dir.y(), dir.z(), 1);

    vertex -> SetPrimary(particle);
    event  -> AddPrimaryVertex(vertex);
  };
}

std::function<void(G4Event*)> geantinos_at_z(G4double z, G4double rmax, G4ThreeVector dir) {
  auto geantino = n4::find_particle("geantino");
  return [z, rmax, dir, geantino](G4Event *event) {
    auto x        = (G4UniformRand() - 0.5) * rmax;
    auto y        = (G4UniformRand() - 0.5) * rmax;
    auto vertex   = new G4PrimaryVertex({x, y, z}, 0.);
    auto particle = new G4PrimaryParticle(geantino, dir.x(), dir.y(), dir.z(), 1);

    vertex -> SetPrimary(particle);
    event  -> AddPrimaryVertex(vertex);
  };
}
