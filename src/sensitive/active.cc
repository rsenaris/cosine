
#include "common.hh"
#include "config.hh"
#include "sensitive/active.hh"
#include "types.hh"

#include <G4OpticalPhoton.hh>
#include <G4ParticleDefinition.hh>
#include <G4Step.hh>

#include <n4-inspect.hh>
#include <n4-sensitive.hh>

#include <vector>


std::vector<IonizationHit> IONIZATION_HITS;

std::unique_ptr<n4::sensitive_detector> sensitive_noble() {
  auto reset_hit_store = [](G4HCofThisEvent *) {
    IONIZATION_HITS.clear();
    IONIZATION_HITS.reserve(LARGE_CHUNK_SIZE);
  };

  auto record_hits = [](G4Step *step) -> bool {
    static auto photondef = dynamic_cast<G4ParticleDefinition*>(G4OpticalPhoton::Definition());

    auto pdef = step -> GetTrack() -> GetDefinition();
    if (pdef == photondef) return false;

    auto deposited_energy = step -> GetTotalEnergyDeposit();
    if (deposited_energy <= 0) return false;

    auto event    = START_ID + n4::event_number();
    auto track_id = step -> GetTrack() -> GetTrackID();
    auto pos      = ( step -> GetPreStepPoint()  -> GetPosition() +
                      step -> GetPostStepPoint() -> GetPosition() ) / 2;

    IONIZATION_HITS.push_back(make_ionization_hit( event, track_id
                                                 , pos.x(), pos.y(), pos.z()
                                                 , deposited_energy));
    return true;
  };

  auto active = std::make_unique<n4::sensitive_detector>("ACTIVE", record_hits);
  active -> initialize(reset_hit_store);
  n4::fully_activate_sensitive_detector(active.get());
  return active;
}
