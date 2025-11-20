#pragma once

#include <G4LogicalVolume.hh>

#include "types.hh"

G4LogicalVolume* build_sipm_array(f64 sipm_size, f64 sipm_thick, f64 sipm_gap, u16 n_sipm_side);
