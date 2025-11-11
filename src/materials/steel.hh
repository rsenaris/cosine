#pragma once

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalSurface.hh>

G4MaterialPropertiesTable* steel_mpt();
G4Material*                steel_with_properties();
G4OpticalSurface*          steel_surface();
