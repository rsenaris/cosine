#pragma once

#include <G4ThreeVector.hh>
#include <G4Event.hh>

#include <functional>

std::function<void(G4Event*)> geantinos(G4ThreeVector pos);
std::function<void(G4Event*)> geantinos(G4ThreeVector pos, G4ThreeVector dir);
