#pragma once

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalSurface.hh>

G4double LAr_Scintillation(G4double energy);
G4double LAr_refractive_index(G4double energy);

G4MaterialPropertiesTable* LAr_mpt();
G4Material*                LAr_with_properties();
G4OpticalSurface*          LAr_surface();
