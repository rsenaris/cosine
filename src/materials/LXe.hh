#pragma once

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalSurface.hh>

G4double LXe_Scintillation(G4double energy);
G4double LXe_refractive_index(G4double energy);

G4MaterialPropertiesTable* LXe_mpt();
G4Material*                LXe_with_properties();
G4OpticalSurface*          LXe_surface();
