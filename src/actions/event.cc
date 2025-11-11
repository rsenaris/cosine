#include "actions/event.hh"
#include "actions/store_volume_crossing.hh"
#include "persistency/manager.hh"
#include "sensitive/sipm.hh"
#include "types.hh"

#include <vector>

static EventCounter EVENT_COUNTER{};

std::vector<SensorHit> SENSOR_HITS;

std::function<void(const G4Event*)> count_event() {
  return [](const G4Event *) {
    EVENT_COUNTER.next();
  };
}

std::function<void(const G4Event*)> store_event() {
  return [](const G4Event *) {
    auto writer = PERSISTENCY_MANAGER.get() -> writer();
    writer -> write_steps(std::move(VOLUME_CHANGES));
    writer -> write_hits(std::move(SENSOR_HITS));
  };
}
