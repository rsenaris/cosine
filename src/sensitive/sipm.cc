#include "G4HCofThisEvent.hh"
#include "common.hh"
#include "types.hh"
#include "sensitive/sipm.hh"

#include <G4OpticalPhoton.hh>
#include <G4ParticleDefinition.hh>
#include <G4Step.hh>
#include <G4VTouchable.hh>
#include <n4-inspect.hh>
#include <n4-sensitive.hh>

G4int get_sensor_id(const G4VTouchable* touch) {
  /// The sensors are placed in a support volume. The support might be
  /// replicated multiple times. Thus, we need to combine this information to
  /// avoid duplicate IDs

  auto   mother_id = touch -> GetCopyNumber(0); // copy number of support
  auto daughter_id = touch -> GetCopyNumber(1); // copy number of sensor
  return mother_id * 1000 + daughter_id;
}

std::unique_ptr<n4::sensitive_detector> sensitive_sipm() {
  sensor_hits.reserve(LARGE_CHUNK_SIZE);

  auto reset_hit_store = [](G4HCofThisEvent*) { sensor_hits.clear(); };

  auto record_hits = [](G4Step *step) -> bool {
    static G4ParticleDefinition* photon = dynamic_cast<G4ParticleDefinition*>(G4OpticalPhoton::Definition());

    auto pdef = step -> GetTrack() -> GetDefinition();
    if (pdef != photon) return false;

    auto event = n4::event_number();
    auto sensor_id = get_sensor_id(step -> GetPostStepPoint() -> GetTouchable());
    auto time = step -> GetPostStepPoint() -> GetGlobalTime();
    sensor_hits.push_back(make_sensor_hit(event, sensor_id, time));
    return true;
  };

  auto sipm = std::make_unique<n4::sensitive_detector>("SiPM", record_hits);
  sipm -> end_of_event(reset_hit_store);
  n4::fully_activate_sensitive_detector(sipm.get());
  return sipm;
}
