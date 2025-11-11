#pragma once

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalSurface.hh>

G4MaterialPropertiesTable* silicon_mpt();
G4Material*                silicon_with_properties();
G4OpticalSurface*          silicon_surface();
