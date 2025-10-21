#include "common.hh"
#include "materials/steel.hh"

#include <n4-material.hh>

G4Material *steel_with_properties(){

  auto steel = n4::material("G4_STAINLESS-STEEL");
  auto reflectivity = 0.20;
  auto mpt = n4::material_properties()
    .add("REFLECTIVITY", OPTPHOT_E_RANGE, reflectivity)
    .done();

  steel->SetMaterialPropertiesTable(mpt);
  return steel;
}
