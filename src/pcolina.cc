#include <G4SystemOfUnits.hh>
#include "G4Color.hh"
#include "G4Colour.hh"
#include "LXe.hh"
#include "sipm_array.hh"
#include "ptfe.hh"
#include "steel.hh"

#include <n4-geometry.hh>
#include <n4-vis-attributes.hh>

#include "mesh.hh"


auto pcolina() {
  auto el_diam         = 32 * mm;
  auto el_r            = el_diam / 2.0;
  auto drift_length    = 120 * mm;
  auto form_factor     = 1.0; // ratio between drift length and cathode radius
  auto cath_diam       = form_factor * drift_length + el_diam;
  auto cath_r          = cath_diam / 2.0;
  auto wall_thick      = 1 * mm;
  auto d_gate_wire     = 5 * mm;
  auto d_wire_shield   = 5 * mm;
  auto mesh_wire_pitch = 0.5 * mm;
  auto mesh_wire_diam  = 0.05 * mm;
  auto neck_length     = d_gate_wire + d_wire_shield + mesh_wire_diam;
  auto sipm_size       = 6 * mm;
  auto sipm_thick      = 1 * mm;
  auto sipm_gap        = 0.5 * mm;
  auto n_sipm_side     = 5;
  auto cath_thick      = 3 * sipm_thick;

  auto air   = n4::material("G4_AIR");
  auto lxe   = LXe_with_properties();
  auto ptfe  = ptfe_with_properties();
  auto steel = steel_with_properties();

  auto tred      = G4Colour {1, 0 ,0, 0.1 };
  auto invisible = n4::vis_attributes().visible(false);
  auto white     = n4::vis_attributes().visible(true).color(G4Color::White());
  auto gray      = n4::vis_attributes().visible(true).color(G4Color::Gray());
  auto red       = n4::vis_attributes().visible(true).color(tred);
  auto frame     = n4::vis_attributes().visible(true).color(G4Color::Grey ()).force_wireframe(true);

  auto world = n4::box("world")
    .cube(1.2 * cath_diam)
    .z(2.5 * drift_length)
    .vis(frame)
    .volume(air);

  auto liquid_length = (drift_length + neck_length) * 2.1;
  auto liquid = n4::box("liquid")
    .cube(1.1 * cath_diam)
    .z(liquid_length)
    .vis(red)
    .place(lxe)
    .in(world)
    .now();

  n4::cons("walls")
    .r1_inner(el_r)
    .r2_inner(cath_r)
    .r_delta(wall_thick)
    .z(drift_length)
    .vis(white)
    .place(ptfe)
    .in(liquid)
    .at_z(drift_length/2)
    .now();

  n4::tubs("el_wall")
    .r_inner(el_r)
    .r_delta(wall_thick)
    .z(neck_length)
    .vis(white)
    .place(ptfe)
    .at_z(-neck_length / 2)
    .in(liquid)
    .now();

  auto mesh_el = create_mesh(el_diam, mesh_wire_pitch, mesh_wire_diam);
  mesh_el -> SetVisAttributes(invisible);

  auto cathode = n4::tubs("cathode")
    .r(cath_r + wall_thick)
    .z(3 * sipm_thick)
    .vis(gray)
    .place(steel)
    .at_z(drift_length + cath_thick / 2)
    .in(liquid)
    .now();

  n4::place(mesh_el)                                       .in(liquid).now(); // GATE
  n4::place(mesh_el).at_z(- neck_length + mesh_wire_diam/2).in(liquid).now(); // SHIELD

  auto sipm_array = build_sipm_array(sipm_size, sipm_thick, sipm_gap, n_sipm_side);
  n4::place(sipm_array)
    .at_z(-neck_length -sipm_thick)
    .in(liquid)
    .copy_no(0)
    .name("near_plane")
    .now();

  auto delta = (sipm_size + sipm_gap) * n_sipm_side / 2.;
  auto n = 1;
  for   (auto x : {-delta, delta}) {
    for (auto y : {-delta, delta}) {
      n4::place(sipm_array)
        .rot_y(180 * deg)
        .at(x, y, -cath_thick/2 + sipm_thick)
        .in(cathode)
        .copy_no(n++)
        .name("far_plane")
        .now();
    }
  }

  return n4::place(world).now();
}
