#pragma once

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalSurface.hh>

G4MaterialPropertiesTable* ptfe_mpt();
G4Material*                ptfe_with_properties();
G4OpticalSurface*          ptfe_surface();
