#pragma once

#include "G4LogicalVolume.hh"

G4LogicalVolume* build_sipm_array(G4double sipm_size, G4double sipm_thick, G4double sipm_gap, G4int n_sipm_side);
