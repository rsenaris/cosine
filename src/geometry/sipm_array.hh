#pragma once

#include "core/types.hh"

#include <G4LogicalVolume.hh>

G4LogicalVolume* build_sipm_array(f64 sipm_size, f64 sipm_thick, f64 sipm_gap, u16 n_sipm_side);
