#include "actions/event.hh"
#include "G4PrimaryVertex.hh"
#include "actions/store_volume_crossing.hh"
#include "persistency/manager.hh"
#include "sensitive/sipm.hh"
#include "types.hh"

#include <vector>

static EventCounter EVENT_COUNTER{};

std::vector<SensorHit> SENSOR_HITS;

static Interaction PRIMARIES;

std::function<void(const G4Event*)> count_event() {
  return [](const G4Event *) {
    EVENT_COUNTER.next();
  };
}

std::function<void(const G4Event*)> store_primaries() {
  return [](const G4Event *event) {
    auto vertex = event -> GetPrimaryVertex();
    auto n      = vertex -> GetNumberOfParticle();
    auto pos    = vertex -> GetPosition();
    auto e      = 0e0;
    auto k      = 0;
    while (true) {
      auto p = vertex -> GetPrimary(k++);
      if (!p) break;
      e += p -> GetKineticEnergy();
    }
    e /= static_cast<f32>(n);
    PRIMARIES = make_interaction(pos, e, n);
  };
}



std::function<void(const G4Event*)> store_event() {
  return [](const G4Event *) {
    auto writer = PERSISTENCY_MANAGER.get() -> writer();
    writer -> write_steps(std::move(VOLUME_CHANGES));
    writer -> write_hits(std::move(SENSOR_HITS));
    writer -> write_interaction(std::move(PRIMARIES));
  };
}
