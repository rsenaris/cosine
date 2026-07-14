#include "core/common.hh"
#include "materials/LAr.hh"
#include "materials/LXe.hh"
#include "materials/LArXe.hh"

#include <G4SystemOfUnits.hh> // physical units such as `m` for metre

#include <n4-constants.hh>
#include <n4-material.hh>
#include <n4-random.hh>
#include <n4-sequences.hh>


std::tuple<G4double, G4double, G4double> LArXe_fractions(G4double xe_fraction) {

  auto fractions = n4::scale_by(1.0, {0.1, 1.0, 2.0, 5.0, 10., 100., 1000.});
  auto total     = n4::scale_by(1.0, {25637.75510, 25551.64209, 26122.79197, 27381.68839,
                                      28335.45918, 29703.42780, 29933.43723});
  auto xenon     = n4::scale_by(1.0, {  229.55968,  3128.20485,  6645.71900, 12840.90422,
                                      17200.28726, 23944.49923, 27551.02041});
  auto singlet   = n4::scale_by(1.0, { 5931.15460,  5921.57220,  5918.64603, 5876.68282,
                                       5835.45918,  5175.36658,  2372.44898});

  auto triplet   = std::vector<G4double>{};
  for (int i = 0; i < fractions.size(); i++) {
    xenon[i]   /= total[i];
    singlet[i] /= total[i];

    triplet.push_back(1.0 - xenon[i] - singlet[i]);
    if (triplet.back() < 0.0) {
      G4Exception("[fractions]", "", FatalException, "invalid fraction");
    }
  }

  // constant extrapolation to higher concentrations
  fractions.push_back(     1000000.0);
  total    .push_back(  total.back());
  xenon    .push_back(  xenon.back());
  singlet  .push_back(singlet.back());
  triplet  .push_back(triplet.back());

  for (int i = fractions.size() - 2; i >= 0; i--) {
    auto dx_i = xe_fraction - fractions[i];
    if (dx_i < 0) { continue; }

    auto dx         = fractions[i+1] - fractions[i];
    auto dy_xenon   =     xenon[i+1] -     xenon[i];
    auto dy_singlet =   singlet[i+1] -   singlet[i];
    auto dy_triplet =   triplet[i+1] -   triplet[i];

    auto y_xenon   =   xenon[i] + dy_xenon  /dx * dx_i;
    auto y_singlet = singlet[i] + dy_singlet/dx * dx_i;
    auto y_triplet = triplet[i] + dy_triplet / dx * dx_i;

    return std::make_tuple(y_singlet, y_triplet, y_xenon);
  }
  G4Exception("[LArXe_fractions]", "", FatalException, "input fraction below range");

  // Thanks to C++'s keen grasp of control flow, we need a return value at this
  // entirely unreachable point.
  return std::make_tuple(0,0,0);
}

G4MaterialPropertiesTable* LArXe_mpt(G4double xe_fraction) {
  /// Scintillation yield from https://iopscience.iop.org/article/10.1143/JJAP.41.1538
  /// Scintillation components from https://arxiv.org/pdf/1511.07721

  auto [sc_energies_ar, sc_values_ar] = n4::interpolate(LAr_Scintillation   , 500, c4::hc/(140*nm), c4::hc/(110*nm));
  auto [sc_energies_xe, sc_values_xe] = n4::interpolate(LXe_Scintillation   , 500, c4::hc/(185*nm), c4::hc/(150*nm));
  auto [ri_energies   , ri_values   ] = n4::interpolate(LAr_refractive_index, 200, OPTPHOT_MIN_E, OPTPHOT_MAX_E);

  auto [singlet, tripet, xenon] = LArXe_fractions(xe_fraction);

  return n4::material_properties()
    .add("RINDEX"                     , ri_energies, ri_values)
    .add("ABSLENGTH"                  , OPTPHOT_E_RANGE, {NO_ABSORPTION, NO_ABSORPTION})
    .add("RESOLUTIONSCALE"            ,     1        )
    .NEW("YIELDRATIO"                 ,     0.03     )
    .NEW("RAYLEIGH"                   ,    60   * cm )
    .NEW("ATTACHMENT"                 ,  1000   * ms )

    .add("SCINTILLATIONYIELD"         , 1.0 / (17.1 * eV))
    .add("SCINTILLATIONCOMPONENT1"    , sc_energies_ar, sc_values_ar ) // spectrum
    .add("SCINTILLATIONCOMPONENT2"    , sc_energies_ar, sc_values_ar ) // spectrum
    .add("SCINTILLATIONCOMPONENT3"    , sc_energies_xe, sc_values_xe ) // spectrum
    .add("SCINTILLATIONTIMECONSTANT1" ,  4.0 * ns    ) // average of 2 and 6 ns from Xe and Ar
    .add("SCINTILLATIONTIMECONSTANT2" , 1300 * ns    )
    .add("SCINTILLATIONTIMECONSTANT3" , 43.5 * ns    )
    .add("SCINTILLATIONYIELD1"        , singlet      )
    .add("SCINTILLATIONYIELD2"        , tripet       )
    .add("SCINTILLATIONYIELD3"        , xenon        )

    .done();
}

G4Material* LArXe_with_properties(G4double xe_fraction) {
  auto opts = n4::material_options {.state = kStateLiquid};
  auto LArXe  = n4::material_from_elements_N("n4_lAr", 1.3954*g/cm3, opts, {{"Ar", 1}});
  LArXe -> SetMaterialPropertiesTable(LArXe_mpt(xe_fraction));
  return LArXe;
}

G4OpticalSurface *LArXe_surface(G4double xe_fraction) {
  auto surface = new G4OpticalSurface("LArXe surface", unified, ground, dielectric_dielectric, .01);
  surface -> SetMaterialPropertiesTable(LArXe_mpt(xe_fraction));
  return surface;
}
