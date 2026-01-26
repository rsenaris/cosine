
#include "common.hh"
#include "config.hh"
#include "materials/silicon.hh"
#include "types.hh"
#include "sensitive/sipm.hh"

#include <G4HCofThisEvent.hh>
#include <G4OpticalPhoton.hh>
#include <G4ParticleDefinition.hh>
#include <G4Step.hh>
#include <G4VTouchable.hh>

#include <n4-inspect.hh>
#include <n4-random.hh>
#include <n4-sensitive.hh>

G4int get_sensor_id(const G4VTouchable* touch) {
  /// The sensors are placed in a support volume. The support might be
  /// replicated multiple times. Thus, we need to combine this information to
  /// avoid duplicate IDs. Depth means steps in the volume hierarchy, 0 being
  /// the current volume, 1 the one containing it, etc.

  auto  sensor_id = touch -> GetCopyNumber(0);
  auto support_id = touch -> GetCopyNumber(1);
  return support_id * 1000 + sensor_id;
}

std::unique_ptr<n4::sensitive_detector> sensitive_sipm() {
  SENSOR_HITS.reserve(LARGE_CHUNK_SIZE);

  auto reset_hit_store = [](G4HCofThisEvent *) {
    SENSOR_HITS.clear();
    SENSOR_HITS.reserve(LARGE_CHUNK_SIZE);
  };

  auto record_hits = [](G4Step *step) -> bool {
    static auto photondef = dynamic_cast<G4ParticleDefinition*>(G4OpticalPhoton::Definition());

    auto pdef = step -> GetTrack() -> GetDefinition();
    if (pdef != photondef) return false;

    auto process = step -> GetPostStepPoint()
                        -> GetProcessDefinedStep()
                        -> GetProcessName();
    if (process != "OpAbsorption") return false;

    // Simulate detection quantum efficiency
    static const auto& sipm_qe = silicon_mpt()->GetProperty("QUANTUM_EFFICIENCY");

    auto photon_e = step -> GetTrack() -> GetDynamicParticle() -> GetTotalEnergy();
    auto qe       = sipm_qe -> Value(photon_e);
    if (n4::random::uniform() > qe) return false;

    auto event     = START_ID + n4::event_number();
    auto sensor_id = get_sensor_id(step -> GetPostStepPoint() -> GetTouchable());
    auto time      = step -> GetPostStepPoint() -> GetGlobalTime();
    SENSOR_HITS.push_back(make_sensor_hit(event, sensor_id, time));

    return true;
  };

  auto sipm = std::make_unique<n4::sensitive_detector>("SiPM", record_hits);
  sipm -> initialize(reset_hit_store);
  n4::fully_activate_sensitive_detector(sipm.get());
  return sipm;
}
