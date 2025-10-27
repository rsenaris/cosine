#include "actions/event.hh"
#include "persistency/manager.hh"
#include "types.hh"

#include <vector>

static EventCounter EVENT_COUNTER{};

extern std::vector<StepData> step_data_container;

std::function<void(const G4Event*)> store_event() {
  auto pm     = dynamic_cast<PersistencyManager*>(G4VPersistencyManager::GetPersistencyManager());
  auto writer = pm -> writer();

  writer.write_steps(std::move(step_data_container));

  EVENT_COUNTER.next();
}
