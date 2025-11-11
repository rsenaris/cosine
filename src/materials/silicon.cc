#include "common.hh"
#include "materials/silicon.hh"

#include <n4-material.hh>
#include <n4-sequences.hh>

using CLHEP::eV;

G4MaterialPropertiesTable *silicon_mpt() {
  // reflectivity from https://refractiveindex.info/?shelf=main&book=Si&page=Franta-100K
  auto reflectivity = 0.65;

  auto energies = n4::scale_by(eV, { OPTPHOT_MIN_E_EV, 1.0       , 1.3       , 1.3776022 , 1.4472283 , 1.52041305,
                                           1.59290956, 1.66341179, 1.72546158, 1.78169885, 1.8473836 , 1.90593775,
                                           1.94918431, 1.99443901, 2.05580636, 2.12107005, 2.17476803, 2.23125551,
                                           2.29951572, 2.37208426, 2.46950045, 2.75730000, 3.04467524, 3.14006977,
                                           3.20705792, 3.2592052 , 3.31307637, 3.38773724, 3.44597914, 3.50625866,
                                           3.54763044, 3.58999021, 3.61155171, 3.65546116, 3.67781872, 3.72336446,
                                           3.74656299, 3.79383824, 3.86703126, 4.0       , 4.2       , OPTPHOT_MAX_E_EV });

  auto efficiencies = n4::scale_by(0.01, {0.0       , 0.0       , 0.0       , 0.03506431, 0.0560223 , 0.07944598,
                                          0.10291004, 0.12607905, 0.14954693, 0.17061431, 0.193974  , 0.21740781,
                                          0.23843982, 0.26178898, 0.28501443, 0.30860135, 0.33010702, 0.35361856,
                                          0.37751412, 0.40109223, 0.42743901, 0.43779628, 0.41021884, 0.38111716,
                                          0.36057765, 0.33517116, 0.31491576, 0.28931617, 0.26968918, 0.24455707,
                                          0.22893024, 0.20156831, 0.18538152, 0.15444223, 0.12549548, 0.1040183 ,
                                          0.0902669 , 0.05498552, 0.02944706, 0.0       , 0.0       , 0.0       });

  return n4::material_properties()
    .add("EFFICIENCY", energies, efficiencies)
    .add("REFLECTIVITY", energies, reflectivity)
    .done();
}

G4Material* silicon_with_properties() {
  auto silicon = n4::material("G4_Si");

  silicon -> SetMaterialPropertiesTable(silicon_mpt());
  return silicon;
}

G4OpticalSurface* silicon_surface() {
  auto surface = new G4OpticalSurface("silicon_surface", unified, polished, dielectric_metal);
  surface -> SetMaterialPropertiesTable(silicon_mpt());
  return surface;
}
