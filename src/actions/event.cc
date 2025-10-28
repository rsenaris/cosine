#include "actions/event.hh"
#include "persistency/manager.hh"
#include "types.hh"

#include <vector>

static EventCounter EVENT_COUNTER{};

extern std::vector<StepData> step_data_container;

std::function<void(const G4Event*)> count_event() {
  return [](const G4Event *) {
    EVENT_COUNTER.next();
  };
}

std::function<void(const G4Event*)> store_event() {
  return [](const G4Event *) {
    auto writer = PERSISTENCY_MANAGER.get() -> writer();
    writer -> write_steps(std::move(step_data_container));
  };
}
