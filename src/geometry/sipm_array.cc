#include "geometry/sipm_array.hh"
#include "materials/ptfe.hh"
#include "materials/silicon.hh"
#include "sensitive/sipm.hh"

#include <G4LogicalSkinSurface.hh>

#include <n4-geometry.hh>
#include <n4-vis-attributes.hh>

#include <cstdio>

G4LogicalVolume* build_sipm_array(f64 sipm_size, f64 sipm_thick, f64 sipm_gap, u16 n_sipm_side) {
  auto odd          = n_sipm_side % 2 == 1;
  auto pitch        = sipm_size + sipm_gap;
  auto support_size = pitch * n_sipm_side;
  auto start        = -(n_sipm_side / 2) * pitch;
  start            += odd ? 0 : 0.5*pitch;

  auto silicon = silicon_with_properties();
  auto ptfe    =    ptfe_with_properties();

  auto blue  = n4::vis_attributes().visible(true).color(G4Color::Blue());
  auto brown = n4::vis_attributes().visible(true).color(G4Color::Brown());

  auto support = n4::box("support")
    .xy(support_size)
    .z(2 * sipm_thick)
    .vis(brown)
    .volume(ptfe);

  new G4LogicalSkinSurface("support_surface", support, ptfe_surface());

  auto sens = sensitive_sipm();
  auto sipm = n4::box("sipm")
    .xy(sipm_size)
    .z(sipm_thick)
    .vis(blue)
    .sensitive(sens.release())
    .volume(silicon);

  new G4LogicalSkinSurface("sipm_surface", sipm, silicon_surface());

  auto z = sipm_thick / 2;
  auto n = 0u;
  for   (auto i = 0u; i < n_sipm_side; i++) {
    for (auto j = 0u; j < n_sipm_side; j++) {
      auto x = i * pitch + start;
      auto y = j * pitch + start;
      char name[20];
      std::sprintf(name, "sipm_%d_%d", i, j);
      n4::place(sipm)
        .at(x, y, z)
        .in(support)
        .copy_no(n++)
        .name(name)
        .now();
    }
  }
  return support;
}
