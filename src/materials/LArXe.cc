#include "core/common.hh"
#include "materials/LAr.hh"
#include "materials/LXe.hh"
#include "materials/LArXe.hh"

#include <G4SystemOfUnits.hh> // physical units such as `m` for metre

#include <n4-constants.hh>
#include <n4-material.hh>
#include <n4-random.hh>
#include <n4-sequences.hh>

G4double LArXe_Scintillation(G4double energy) {
  return LXe_Scintillation(energy);
}

G4double LArXe_refractive_index(G4double energy) {
  return LAr_refractive_index(energy);
}


G4MaterialPropertiesTable* LArXe_mpt() {
  /// Scintillation yield from https://iopscience.iop.org/article/10.1143/JJAP.41.1538
  /// Scintillation components from https://arxiv.org/pdf/1511.07721

  // Sampling from ~110 nm to ~140 nm
  auto [sc_energies, sc_values] = n4::interpolate(LArXe_Scintillation   , 500, 8.8571 * eV, 11.2727*eV);
  auto [ri_energies, ri_values] = n4::interpolate(LArXe_refractive_index, 200, OPTPHOT_MIN_E, OPTPHOT_MAX_E);

  return n4::material_properties()
    .add("RINDEX"                     , ri_energies, ri_values)
    .add("ABSLENGTH"                  , OPTPHOT_E_RANGE, {NO_ABSORPTION, NO_ABSORPTION})
    .add("RESOLUTIONSCALE"            ,     1        )
    .NEW("YIELDRATIO"                 ,     0.03     )
    .NEW("RAYLEIGH"                   ,    60   * cm )
    .NEW("ATTACHMENT"                 ,  1000   * ms )

    .add("SCINTILLATIONYIELD"         , 1.0 / (17.1 * eV))
    .add("SCINTILLATIONCOMPONENT1"    , sc_energies, sc_values ) // spectrum
    .add("SCINTILLATIONCOMPONENT2"    , sc_energies, sc_values ) // spectrum
    .add("SCINTILLATIONTIMECONSTANT1" ,  2.0 * ns    )
    .add("SCINTILLATIONTIMECONSTANT2" , 43.5 * ns    )
    .add("SCINTILLATIONYIELD1"        , 0.03         )
    .add("SCINTILLATIONYIELD2"        , 0.97         )

    .done();
}

G4Material* LArXe_with_properties() {
  auto opts = n4::material_options {.state = kStateLiquid};
  auto LArXe  = n4::material_from_elements_N("n4_lAr", 1.3954*g/cm3, opts, {{"Ar", 1}});
  LArXe -> SetMaterialPropertiesTable(LArXe_mpt());
  return LArXe;
}

G4OpticalSurface *LArXe_surface() {
  auto surface = new G4OpticalSurface("LArXe surface", unified, ground, dielectric_dielectric, .01);
  surface -> SetMaterialPropertiesTable(LArXe_mpt());
  return surface;
}
