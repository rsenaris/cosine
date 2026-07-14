#pragma once

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalSurface.hh>

G4double LArXe_Scintillation(G4double energy);
G4double LArXe_refractive_index(G4double energy);

G4MaterialPropertiesTable* LArXe_mpt();
G4Material*                LArXe_with_properties();
G4OpticalSurface*          LArXe_surface();
