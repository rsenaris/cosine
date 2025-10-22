#pragma once

#include <G4Step.hh>

#include <functional>

std::function<void(const G4Step*)> store_volume_crossing(G4String particle, G4String from, G4String to);
