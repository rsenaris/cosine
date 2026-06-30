#include "core/types.hh"
#include "materials/silicon.hh"

#include <G4SystemOfUnits.hh>

#include <n4-constants.hh>
#include <n4-material.hh>
#include <n4-sequences.hh>

#include <algorithm>

using CLHEP::eV;

G4MaterialPropertiesTable *silicon_mpt() {
  auto tiny = 1 * micrometer;

  // refractive index data from
  // https://web.stanford.edu/group/grattalab/Publications/Reflectance_of_Silicon_Photomultipliers_at_Vacuum_Ultraviolet_Wavelengths.pdf

  auto ri_energies = n4::const_over(c4::hc/nm, {
      125.43444, 126.93525, 128.13495, 129.18477, 130.53480, 132.18949, 133.69366,
      135.65064, 137.91364, 140.17746, 142.89786, 146.07368, 149.23594, 152.73611,
      156.06928, 159.40411, 162.73968, 166.07616, 169.41369, 172.75115, 176.08937,
      179.42817, 182.76649, 186.10543, 189.44485, 192.68355, 196.12444, 199.29622,
      202.80488, 206.14511, 209.48576, 212.82623, 216.16669, 219.50802, 225.12829,
      230.29156, 235.16694, 239.24966, 242.59050, 245.93188, 249.27309,
    });

  auto ri_values = n4::scale_by(1.0, {
      2.29258, 2.23857, 2.19255, 2.15254, 2.10185, 2.05370, 2.00969,
      1.95707, 1.91103, 1.86744, 1.82643, 1.78459, 1.75262, 1.72166,
      1.69602, 1.67528, 1.65673, 1.64091, 1.62818, 1.61526, 1.60459,
      1.59562, 1.58525, 1.57670, 1.56961, 1.56411, 1.55761, 1.55389,
      1.54815, 1.54342, 1.53997, 1.53596, 1.53196, 1.53051, 1.53033,
      1.52632, 1.51943, 1.51432, 1.51141, 1.51014, 1.50832,
    });
  std::reverse(ri_energies.begin(), ri_energies.end());
  std::reverse(ri_values  .begin(), ri_values  .end());

  auto qe_energies = n4::const_over(c4::hc/nm, {
              122.73399, 125.29396, 127.73758, 130.18119, 132.62480, 135.06841,
              137.62839, 140.18836, 142.74834, 145.30831, 147.86829, 150.42826,
              152.98823, 155.54821, 157.99182, 159.06289, 160.43543, 161.72626,
              162.41360, 163.69358, 164.38964, 165.55538, 168.11536, 170.67533,
              173.23531, 175.79528, 178.35526, 180.91523, 183.47521, 186.03518,
              188.59515, 191.15513, 193.71510, 196.27508, 198.83505,
    });

  auto qe_values = n4::scale_by( 0.01, {
                11.30009, 12.59709, 14.27306, 15.99540, 17.73223, 19.45167,
                20.87910, 22.12539, 23.38617, 23.95134, 23.82092, 23.50935,
                23.00938, 21.87178, 19.97192, 19.05405, 18.62564, 18.71622,
                19.70890, 21.74136, 22.70270, 23.29197, 23.55282, 23.55282,
                23.71223, 23.81367, 23.81367, 23.74846, 23.73397, 23.73397,
                23.73397, 23.73397, 23.73397, 23.73397, 23.73397,
    });
  std::reverse(qe_energies.begin(), qe_energies.end());
  std::reverse(qe_values  .begin(), qe_values  .end());

  return n4::material_properties()
    // .add("EFFICIENCY" , energies, efficiency)    // this is doing nothing, as it
                                                    // applies only to the skin which has no reflectivity set
    // .add("REFLECTIVITY", energies, reflectivity) // ignored. relying on refractive indices
    .NEW("QUANTUM_EFFICIENCY", qe_energies, qe_values)    // we implement an effective QE for detection efficiency
    .add("ABSLENGTH"         , ri_energies,      tiny)
    .add("RINDEX"            , ri_energies, ri_values)
    .done();
}

G4Material* silicon_with_properties() {
  auto silicon = n4::material("G4_Si");

  silicon -> SetMaterialPropertiesTable(silicon_mpt());
  return silicon;
}

G4OpticalSurface* silicon_surface() {
  auto surface = new G4OpticalSurface("silicon_surface", unified, polished, dielectric_dielectric);
  surface -> SetMaterialPropertiesTable(silicon_mpt());
  return surface;
}
