#include "core/common.hh"
#include "materials/ptfe.hh"

#include <G4OpticalSurface.hh>

#include <n4-material.hh>
#include <n4-sequences.hh>

G4MaterialPropertiesTable* ptfe_mpt() {
  // Values could be taken from "Optical properties of Teflon AF amorphous
  // fluoropolymers" by Yang, French & Tokarsky (using AF2400, Fig.6) but are
  // also stated in the same paper as above
  auto refl_values = n4::scale_by(0.01, {98, 98, 98, 98, 72, 72, 72});
  auto refl_fixed = 0.97;
  auto refl_energies = n4::scale_by(  eV, {OPTPHOT_MIN_E_EV, 2.8, 3.5, 4., 6., 7.2, OPTPHOT_MAX_E_EV});
  auto ref_index     = 1.58; // 1.35 in other projects, 1.41 in nexus

  return n4::material_properties()
      .add("RINDEX", OPTPHOT_E_RANGE, ref_index)
      .add("REFLECTIVITY", refl_energies, refl_fixed)
      .add("SPECULARLOBECONSTANT", OPTPHOT_E_RANGE, 0.)
      .add("SPECULARSPIKECONSTANT", OPTPHOT_E_RANGE, 0.)
      .add("BACKSCATTERCONSTANT", OPTPHOT_E_RANGE, 0.)
      .done();
}

G4Material* ptfe_with_properties() {
  auto teflon = n4::material("G4_TEFLON");
  teflon -> SetMaterialPropertiesTable(ptfe_mpt());
  return teflon;
}

G4OpticalSurface* ptfe_surface() {
  auto surface = new G4OpticalSurface("ptfe surface", unified, ground, dielectric_metal, .01);
  surface -> SetMaterialPropertiesTable(ptfe_mpt());
  return surface;
}
