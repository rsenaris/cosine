#pragma once

#include <G4LogicalVolume.hh>

G4LogicalVolume* create_wire_mesh(G4double diam, G4double pitch, G4double wire_diam);
G4LogicalVolume* create_hex_mesh(G4double frame_diam, G4double frame_thick, G4double frame_width, G4double pitch, G4double mesh_thick, G4double mesh_circumradius);
