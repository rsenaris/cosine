#pragma once

#include <G4LogicalVolume.hh>

G4LogicalVolume* create_wire_array(G4double frame_diam, G4double frame_thick, G4double frame_width, G4double pitch, G4double wire_diam);
