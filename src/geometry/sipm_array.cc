#include "geometry/sipm_array.hh"
#include "materials/ptfe.hh"
#include "materials/silicon.hh"
#include "sensitive/sipm.hh"

#include <G4LogicalSkinSurface.hh>

#include <n4-geometry.hh>
#include <n4-vis-attributes.hh>

#include <cstdio>
#include <cmath>

G4LogicalVolume* build_sipm_array(f64 sipm_size, f64 sipm_thick, f64 sipm_gap, u16 n_sipm_side) {
  auto odd          = n_sipm_side % 2 == 1;
  auto pitch        = sipm_size + sipm_gap;
  auto support_size = pitch * n_sipm_side;
  auto start        = -(n_sipm_side / 2) * pitch;//We start on bottom left corner
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

    //sipm -> SetVisAttributes(n4::vis_attributes().visible(false));

  new G4LogicalSkinSurface("sipm_surface", sipm, silicon_surface());

  auto z = -sipm_thick / 2; //We put the SiPMs on the bottom half of the support (we can easily invert it with a rotation)
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



G4LogicalVolume* build_sipm_array_hexagonal(f64 sipm_size, f64 sipm_thick, f64 sipm_circumradius, f64 support_r) {
  auto silicon = silicon_with_properties();
  auto ptfe    =    ptfe_with_properties();

  auto blue  = n4::vis_attributes().visible(true).color(G4Color::Blue());
  auto brown = n4::vis_attributes().visible(true).color(G4Color::Brown());

  auto support = n4::tubs("support")
    .r(support_r)
    .z(2 * sipm_thick)
    .vis(brown)
    .volume(ptfe);

  new G4LogicalSkinSurface("support_surface", support, ptfe_surface());

  auto sens = sensitive_sipm();//defined in sipm.cc
  auto sipm = n4::box("sipm")
    .xy(sipm_size)
    .z(sipm_thick)
    .vis(blue)
    .sensitive(sens.release())
    .volume(silicon);

  new G4LogicalSkinSurface("sipm_surface", sipm, silicon_surface());

  auto z = -sipm_thick / 2; //We put the SiPMs on the bottom half of the support (we can easily invert it with a rotation)

  for (auto i = 0u; i<6; i++) {
    auto theta = i * CLHEP::pi / 3;
    auto x = sipm_circumradius * std::cos(theta);
    auto y = sipm_circumradius * std::sin(theta);
    char name[20];
    std::sprintf(name, "sipm_%d", i);
    n4::place(sipm)
      .at(x,y,z)
      .in(support)
      .copy_no(i)
      .name(name)
        .now();
  }
  return support;
}


G4LogicalVolume* build_sipm_array_2x2(f64 block_size, f64 block_thick, f64 sipm_size, f64 sipm_thick, f64 sipm_gap, f64 sipm_side_gap, u16 row_idx, u16 column_idx, u16 n_blocks_side) {

  //auto support_side = 2 * (sipm_size + sipm_side_gap) + sipm_gap; 
  auto pitch = (sipm_size + sipm_gap);
  auto start =  - 0.5 * pitch;


  auto silicon = silicon_with_properties();
  auto ptfe    =    ptfe_with_properties();

  auto blue  = n4::vis_attributes().visible(true).color(G4Color::Blue());
  auto brown = n4::vis_attributes().visible(true).color(G4Color::Brown());


  auto support = n4::box("support")
    .xy(block_size)
    .z(block_thick)
    .vis(brown)
    .volume(ptfe);

  new G4LogicalSkinSurface("support_surface", support, ptfe_surface());


  auto sens = sensitive_sipm();//defined in sipm.cc
  auto sipm = n4::box("sipm")
    .xy(sipm_size)
    .z(sipm_thick)
    .vis(blue)
    .sensitive(sens.release())
    .volume(silicon);

  new G4LogicalSkinSurface("sipm_surface", sipm, silicon_surface());


  auto z = -(block_thick - sipm_thick) / 2.; //We put the SiPMs on the bottom half of the support (we can easily invert it with a rotation)
  // auto n = 0u;
  for   (auto i = 0u; i < 2; i++) {
    for (auto j = 0u; j < 2; j++) {
      auto x = i * pitch + start;
      auto y = j * pitch + start;
      // char name[20];
      auto copy_no = (2 * row_idx + i) * n_blocks_side + 2 * column_idx + j;
      n4::place(sipm)
        .at(x, y, z)
        .in(support)
        .copy_no(copy_no)
        // .name(name)
        .now();
    }
  }
  return support;
}


G4LogicalVolume* build_segmentated_sipm_array(u16 n_blocks_side, f64 block_gap, f64 block_size,f64 block_thick, f64 sipm_size, f64 sipm_thick, f64 sipm_gap, f64 sipm_side_gap, G4Material* medium)  {
  auto invisible = n4::vis_attributes().visible(false);
  // auto odd          = n_blocks_side % 2 == 1;
  auto pitch = block_size + block_gap;
  auto start =  -0.5 * (n_blocks_side - 1) * pitch;

  auto array_side = n_blocks_side * block_size + (n_blocks_side + 1) * block_gap;
  auto invis_support = n4::box("segmented_sipm_array")
  .xy(array_side)
  .z(block_thick)
  .vis(invisible)
  .volume(medium);

  
  for (auto i = 0u; i < n_blocks_side; i++) {
    for (auto j = 0u; j< n_blocks_side; j++)  {
      auto block = build_sipm_array_2x2(block_size, block_thick, sipm_size, sipm_thick, sipm_gap, sipm_side_gap, i, j, n_blocks_side);
      auto x = i * pitch + start;
      auto y = j * pitch + start;

      n4::place(block)
        .at(x, y, 0)
        .in(invis_support)
        .now();
    }
  }
  return invis_support;
}