#include "actions/stacking.hh"
#include "G4ClassificationOfNewTrack.hh"

#include <G4OpticalPhoton.hh>

std::function<G4ClassificationOfNewTrack(G4Track const *)> kill_photons() {
  return [](G4Track const* track) {
    static auto opticalphoton = G4OpticalPhoton::Definition();

    auto pdef = track -> GetParticleDefinition();

    return (pdef == opticalphoton)            ?
           G4ClassificationOfNewTrack::fKill  :
           G4ClassificationOfNewTrack::fUrgent;
  };
}
