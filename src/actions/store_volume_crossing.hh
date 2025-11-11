#pragma once

#include "types.hh"
#include <G4Step.hh>

#include <functional>

std::function<void(const G4Step*)> store_volume_crossing(G4String particle, G4String from, G4String to);

struct StoreVolumeCrossing {
  std::vector<VolumeChange> steps;

   StoreVolumeCrossing() : steps() {};
  ~StoreVolumeCrossing(){ steps.clear();};
};

extern std::vector<VolumeChange> VOLUME_CHANGES;
