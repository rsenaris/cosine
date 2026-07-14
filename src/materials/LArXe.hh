#pragma once

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalSurface.hh>

#include <tuple>


G4double LArXe_Scintillation(G4double energy);
G4double LArXe_refractive_index(G4double energy);

std::tuple<G4double, G4double, G4double> LArXe_fractions(G4double xe_fraction);
G4MaterialPropertiesTable*               LArXe_mpt(G4double xe_fraction);
G4Material*                              LArXe_with_properties(G4double xe_fraction);
G4OpticalSurface*                        LArXe_surface(G4double xe_fraction);
