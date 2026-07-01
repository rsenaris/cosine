#include "core/common.hh"
#include "core/utils.hh"
#include "materials/LAr.hh"

#include <G4SystemOfUnits.hh> // physical units such as `m` for metre

#include <n4-constants.hh>
#include <n4-material.hh>
#include <n4-sequences.hh>


G4double LAr_Scintillation(G4double energy) {
  // From Ion-Beam Excitation of Liquid Argon (M.Hofmann et al)
  // https://arxiv.org/pdf/1511.07721
  G4double lambda_peak  = 126.8 * nm;
  G4double lambda_FWHM  =   7.8 * nm;
  G4double lambda_sigma = lambda_FWHM / 2.35;

  G4double E_peak  = c4::hc / lambda_peak;
  G4double E_sigma = E_peak * lambda_sigma / lambda_peak;
  return gaussian(energy, 1.0, E_peak, E_sigma);
}

G4double LAr_refractive_index(G4double energy) {
  // Expression from https://refractiveindex.info/?shelf=main&book=Ar&page=Grace-liquid-90K
  auto wl_in_um = c4::hc / energy / um;
  auto n = std::sqrt( 1.26
                            + 0.2300 / (1.0 - std::pow(0.1066 / wl_in_um, 2))
                            + 0.0023 / (1.0 - std::pow(0.9083 / wl_in_um, 2)));
  if (n < 1) {
    std::printf("LAr refractive index < 1 for %f eV = %f nm\n", energy / eV, wl_in_um * 1e3);
    return 1.62;
  }

  return n;
}


G4MaterialPropertiesTable* LAr_mpt() {
  /// Scintillation yield from https://iopscience.iop.org/article/10.1143/JJAP.41.1538
  /// Scintillation components from https://arxiv.org/pdf/1511.07721

  // Sampling from ~110 nm to ~140 nm
  auto [sc_energies, sc_values] = n4::interpolate(LAr_Scintillation   , 500, 8.8571 * eV, 11.2727*eV);
  auto [ri_energies, ri_values] = n4::interpolate(LAr_refractive_index, 200, OPTPHOT_MIN_E, OPTPHOT_MAX_E);

  return n4::material_properties()
    .add("RINDEX"                     , ri_energies, ri_values)
    .add("ABSLENGTH"                  , OPTPHOT_E_RANGE, {NO_ABSORPTION, NO_ABSORPTION})
    .add("RESOLUTIONSCALE"            ,     1        )
    .NEW("YIELDRATIO"                 ,     0.03     )
    .NEW("RAYLEIGH"                   ,    60   * cm )
    .NEW("ATTACHMENT"                 ,  1000   * ms )

    .add("SCINTILLATIONYIELD"         , 1 / (24.4 * eV))
    .add("SCINTILLATIONCOMPONENT1"    , sc_energies, sc_values ) // spectrum
    .add("SCINTILLATIONCOMPONENT2"    , sc_energies, sc_values ) // spectrum
    .add("SCINTILLATIONTIMECONSTANT1" ,  6.0 * ns    )
    .add("SCINTILLATIONTIMECONSTANT2" , 1300 * ns    )
    .add("SCINTILLATIONYIELD1"        , 0.75         )
    .add("SCINTILLATIONYIELD2"        , 0.25         )

    .done();
}

G4Material* LAr_with_properties() {
  auto opts = n4::material_options {.state = kStateLiquid};
  auto LAr  = n4::material_from_elements_N("n4_lAr", 1.3954*g/cm3, opts, {{"Ar", 1}});
  LAr -> SetMaterialPropertiesTable(LAr_mpt());
  return LAr;
}

G4OpticalSurface *LAr_surface() {
  auto surface = new G4OpticalSurface("LAr surface", unified, ground, dielectric_dielectric, .01);
  surface -> SetMaterialPropertiesTable(LAr_mpt());
  return surface;
}
