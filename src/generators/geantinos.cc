#include "generators/geantinos.hh"
#include "G4RandomDirection.hh"
#include "G4ThreeVector.hh"
#include "generators/pointlike.hh"

#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>
#include <Randomize.hh>

#include <n4-inspect.hh>
#include <n4-random.hh>

std::function<void(G4Event *)> geantinos(G4ThreeVector pos) {
  auto gen = pointlike("geantino", pos, 1, 1);
  return [gen](G4Event *event) {
    event  -> AddPrimaryVertex(gen.generate_vertex());
  };
}


std::function<void(G4Event*)> geantinos(G4ThreeVector pos, G4ThreeVector dir) {
  auto geantino = n4::find_particle("geantino");
  return [pos, dir, geantino](G4Event* event) {
    auto vertex   = new G4PrimaryVertex(pos, 0.);
    auto particle = new G4PrimaryParticle(geantino, dir.x(), dir.y(), dir.z(), 1);

    vertex -> SetPrimary(particle);
    event  -> AddPrimaryVertex(vertex);
  };
}
