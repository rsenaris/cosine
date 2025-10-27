#pragma once

#include "types.hh"
#include <G4Step.hh>

#include <functional>

std::function<void(const G4Step*)> store_volume_crossing(G4String particle, G4String from, G4String to);

struct StoreVolumeCrossing {
  std::vector<StepData> steps;

   StoreVolumeCrossing() : steps() {};
  ~StoreVolumeCrossing(){ steps.clear();};
};

extern std::vector<StepData> step_data_container;
