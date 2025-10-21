#pragma once

#include <G4ThreeVector.hh>
#include <G4Event.hh>

#include <functional>

std::function<void(G4Event*)> geantinos(G4ThreeVector pos, G4ThreeVector dir);
std::function<void(G4Event*)> geantinos_at_z(G4double z, G4double rmax, G4ThreeVector dir);
