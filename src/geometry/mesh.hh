#pragma once

#include <G4LogicalVolume.hh>


#include <optional>


G4LogicalVolume* create_wire_mesh(G4double diam, G4double pitch, G4double wire_diam);
G4LogicalVolume* create_hex_mesh(G4double frame_diam, G4double thick, G4double frame_width, G4double pitch, G4double mesh_circumradius, std::optional<G4bool> wire_top = std::nullopt);
