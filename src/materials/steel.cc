#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "common.hh"
#include "materials/steel.hh"

#include <n4-material.hh>

G4MaterialPropertiesTable* steel_mpt(){
  auto reflectivity = 0.20;
  return n4::material_properties()
    .add("REFLECTIVITY", OPTPHOT_E_RANGE, reflectivity)
    .done();
}

G4Material* steel_with_properties(){
  auto steel = n4::material("G4_STAINLESS-STEEL");
  steel->SetMaterialPropertiesTable(steel_mpt());
  return steel;
}

G4OpticalSurface* steel_surface() {
  auto surface = new G4OpticalSurface("steel_surface", unified, polished, dielectric_metal);
  surface -> SetMaterialPropertiesTable(steel_mpt());
  return surface;
}
