#pragma once

#include "core/types.hh"
#include "core/symbols.hh"

#include <G4LogicalVolume.hh>

G4LogicalVolume* build_sipm_array(f64 sipm_size, f64 sipm_thick, f64 sipm_gap, u16 n_sipm_side);
G4LogicalVolume* build_sipm_array_hexagonal(f64 sipm_size, f64 sipm_thick, f64 sipm_circumradius, f64 support_r);
G4LogicalVolume* build_sipm_array_2x2(f64 block_size, f64 block_thick, f64 sipm_size, f64 sipm_thick, f64 sipm_gap, f64 sipm_side_gap, u16 row_idx, u16 column_idx, u16 n_blocks_side);
G4LogicalVolume* build_segmentated_sipm_array(u16 n_blocks_side, f64 block_gap, f64 block_size,f64 block_thick, f64 sipm_size, f64 sipm_thick, f64 sipm_gap, f64 sipm_side_gap, G4Material* medium);
