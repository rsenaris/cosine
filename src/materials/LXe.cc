#include "core/common.hh"
#include "materials/LXe.hh"

#include <G4SystemOfUnits.hh> // physical units such as `m` for metre

#include <n4-material.hh>
#include <n4-sequences.hh>
#include <n4-constants.hh>


G4double LXe_Scintillation(G4double energy) {
  using CLHEP::c_light;   using CLHEP::h_Planck;   using CLHEP::pi;
  // K. Fuji et al., "High accuracy measurement of the emission spectrum of liquid xenon
  // in the vacuum ultraviolet region",
  // Nuclear Instruments and Methods in Physics Research A 795 (2015) 293–297
  // http://ac.els-cdn.com/S016890021500724X/1-s2.0-S016890021500724X-main.pdf?_tid=83d56f0a-3aff-11e7-bf7d-00000aacb361&acdnat=1495025656_407067006589f99ae136ef18b8b35a04
  G4double lambda_peak  = 174.8 * nm;
  G4double lambda_FWHM  =  10.2 * nm;
  G4double lambda_sigma = lambda_FWHM / 2.35;

  G4double E_peak  = (h_Planck * c_light / lambda_peak);
  G4double E_sigma = (h_Planck * c_light * lambda_sigma / pow(lambda_peak, 2));

  G4double intensity = exp(-pow(E_peak / eV - energy / eV, 2) / (2 * pow(E_sigma / eV, 2)))
    / (E_sigma / eV * sqrt(pi * 2.));

  return intensity;
}

G4double LXe_refractive_index(G4double energy) {
  // Expression from https://refractiveindex.info/?shelf=main&book=Xe&page=Grace-liquid-178K
  auto wl_in_um = c4::hc / energy / um;
  auto n = std::sqrt( 1.4
                    + 0.400 / (1.0 - std::pow(0.1469 / wl_in_um, 2))
                    + 0.002 / (1.0 - std::pow(0.8270 / wl_in_um, 2)));
  if (n < 1)
    std::cerr << "LXe refractive index < 1." << std::endl;

  return n;
}


G4MaterialPropertiesTable* LXe_mpt() {
  /// The time constants are taken from E. Hogenbirk et al 2018 JINST 13 P10031

  // Sampling from ~151 nm to 200 nm <----> from 6.20625 eV to 8.21 eV // TODO convert here
  auto [sc_energies, sc_values] = n4::interpolate(LXe_Scintillation   , 500, 6.2*eV, 8.26*eV);
  auto [ri_energies, ri_values] = n4::interpolate(LXe_refractive_index, 200, OPTPHOT_MIN_E, OPTPHOT_MAX_E);

  return n4::material_properties()
    .add("RINDEX"                     , ri_energies, ri_values)
    .add("ABSLENGTH"                  , OPTPHOT_E_RANGE, {NO_ABSORPTION, NO_ABSORPTION})
    .add("RESOLUTIONSCALE"            ,     1        )
    .NEW("YIELDRATIO"                 ,     0.03     )
    .NEW("RAYLEIGH"                   ,    36   * cm )
    .NEW("ATTACHMENT"                 ,  1000   * ms )

    .add("SCINTILLATIONYIELD"         , 58708   / MeV)
    .add("SCINTILLATIONCOMPONENT1"    , sc_energies, sc_values ) // spectrum
    .add("SCINTILLATIONCOMPONENT2"    , sc_energies, sc_values ) // spectrum
    .add("SCINTILLATIONTIMECONSTANT1" ,  2.0 * ns    )
    .add("SCINTILLATIONTIMECONSTANT2" , 43.5 * ns    )
    .add("SCINTILLATIONYIELD1"        , 0.03         )
    .add("SCINTILLATIONYIELD2"        , 0.97         )

    .done();
}

G4Material* LXe_with_properties() {
  auto opts = n4::material_options {.state = kStateLiquid};
  auto LXe  = n4::material_from_elements_N("n4_lXe", 2.98*g/cm3, opts, {{"Xe", 1}});
  LXe -> SetMaterialPropertiesTable(LXe_mpt());
  return LXe;
}

G4OpticalSurface *LXe_surface() {
  auto surface = new G4OpticalSurface("LXe surface", unified, ground, dielectric_dielectric, .01);
  surface -> SetMaterialPropertiesTable(LXe_mpt());
  return surface;
}
