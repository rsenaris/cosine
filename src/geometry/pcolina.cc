#include "geometry/mesh.hh"
#include "geometry/sipm_array.hh"
#include "geometry/wire_array.hh"
#include "materials/LXe.hh"
#include "materials/ptfe.hh"
#include "materials/steel.hh"


#include <G4Color.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4SystemOfUnits.hh>
#include <G4Polyhedra.hh>

#include <n4-geometry.hh>
#include <n4-place.hh>
#include <n4-shape.hh>
#include <n4-volume.hh>
#include <n4-vis-attributes.hh>

#include "config.hh"

auto pcolina(const geometry_config& c) {
  auto full_neck_length = c.neck_length
                        + c.mesh_thick // gate thickness
                        + c.d_gate_wire   - c.frame_thick_wires/2
                        + c.d_wire_shield - c.frame_thick_wires/2
                        + c.mesh_thick // shield thickness
                        + c.d_shield_sipms;

  auto air   = n4::material("G4_AIR");
  auto lxe   =   LXe_with_properties();
  auto ptfe  =  ptfe_with_properties();
  auto steel = steel_with_properties();

  auto tred      = G4Colour {1, 0 ,0, 0.1 };
  auto invisible = n4::vis_attributes().visible(false);
  auto wireframe = n4::vis_attributes().visible(true).color(G4Color::White()).force_wireframe(true).line_width(5);
  auto white     = n4::vis_attributes().visible(true).color(G4Color::White());
  auto green     = n4::vis_attributes().visible(true).color(G4Color::Green());
  auto gray      = n4::vis_attributes().visible(true).color(G4Color::Gray());
  auto red       = n4::vis_attributes().visible(true).color(tred);
  auto frame     = n4::vis_attributes().visible(true).color(G4Color::Grey ()).force_wireframe(true);

  //n4::place::check_overlaps_switch_on();

  auto world = n4::box("world")
    .cube(1.2 * c.cath_diam())
    .z(2.5 * c.drift_length)
    .vis(frame)
    .volume(air);

  auto liquid_length = (c.drift_length + full_neck_length) * 2.1;
  auto liquid = n4::box("liquid")
    .cube(1.1 * c.cath_diam())
    .z(liquid_length)
    .vis(red)
    .place(lxe)
    .in(world)
    .now();

  auto neck = n4::tubs("neck")
    .r_inner(c.el_r())
    .r_delta(c.wall_thick)
    .z(c.neck_length)
    .vis(white)
    .place(ptfe)
    .in(liquid)
    .at_z(c.neck_length/2)
    .now();

  new G4LogicalSkinSurface("neck_surface", neck -> GetLogicalVolume(), ptfe_surface());


  // Field cage rings
  for (auto i=0; i<c.fc_rings; i++) {
    char name[20];
    auto z  = c.fc_ring_zpitch*i + c.fc_ring_width/2;
    auto r1 = c.el_r() + c.form_factor * (z - c.fc_ring_width/2);
    auto r2 = c.el_r() + c.form_factor * (z + c.fc_ring_width/2);
    std::sprintf(name, "field_cage_ring_%d", i);
    auto ring = n4::cons(name)
      .r1_inner(r1)
      .r2_inner(r2)
      .r_delta(c.fc_ring_thick)
      .z(c.fc_ring_width)
      .vis(gray)
      .place(steel)
      .in(liquid)
      .at_z(c.neck_length + z)
      .now();

    char name2[30];
    std::sprintf(name2, "%s_surface", name);
    new G4LogicalSkinSurface(name2, ring -> GetLogicalVolume(), steel_surface());
  }

  if (c.ptfe_on_walls) {
    auto ptfe_walls = n4::cons("ptfe_walls")
      .r1_inner(c.el_r())
      .r2_inner(c.cath_r())
      .r_delta(c.wall_thick)
      .z(c.drift_length)
      .vis(wireframe)
      .place(ptfe)
      .in(liquid)
      .at_z(c.neck_length + c.drift_length/2)
      .now();

    new G4LogicalSkinSurface("cath_surface", ptfe_walls -> GetLogicalVolume(), ptfe_surface());
  }

  auto wire_array = create_wire_array(c);

  wire_array -> SetVisAttributes(gray);
  n4::place(wire_array)
    .rot_z(c.thin_wire_rot)
    .at_z(-c.mesh_thick - c.d_gate_wire)
    .in(liquid)
    .now();

  auto mesh_el = create_hex_mesh( c.el_diam
                                , c.frame_thick_mesh
                                , c.frame_width
                                , c.mesh_hex_pitch
                                , c.mesh_thick
                                , c.mesh_hex_inradius);
  mesh_el -> SetVisAttributes(invisible);
  // mesh_el -> SetVisAttributes(green);

  n4::place(mesh_el).name("gate"  ).at_z(-c.mesh_thick/2                                              ).in(liquid).now();
  n4::place(mesh_el).name("shield").at_z(-c.mesh_thick -c.d_gate_wire -c.d_wire_shield -c.mesh_thick/2).in(liquid).now();

  auto z_cathode = c.neck_length + c.drift_length + c.cath_thick/2;
  z_cathode += c.ptfe_on_fp ? c.wall_thick : 0;
  auto cathode = n4::tubs("cathode")
    .r(c.cath_r() + c.wall_thick)
    .z(c.cath_thick)
    .vis(gray)
    .place(steel)
    .at_z(z_cathode)
    .in(liquid)
    .now();

  new G4LogicalSkinSurface("cathode_surface", cathode -> GetLogicalVolume(), steel_surface());

  if (c.ptfe_on_fp) {
    auto ptfe_cathode = n4::tubs("ptfe_cathode")
      .r(c.cath_r() + c.wall_thick)
      .z(c.wall_thick)
      .vis(white)
      .place(ptfe)
      .at_z(z_cathode -c.cath_thick/2 -c.wall_thick/2)
      .in(liquid)
      .now();

    new G4LogicalSkinSurface("cath_surface", ptfe_cathode -> GetLogicalVolume(), ptfe_surface());
  }

  auto sipm_array = build_sipm_array(c.sipm_size, c.sipm_thick, c.sipm_gap, c.n_sipm_side);
  n4::place(sipm_array)
    .at_z(-full_neck_length -c.sipm_thick)
    .in(liquid)
    .copy_no(0)
    .name("near_plane")
    .now();

  if (c.sipms_on_fp) {
    auto delta = (c.sipm_size + c.sipm_gap) * c.n_sipm_side / 2.;
    auto n = 1;
    for   (auto x : {-delta, delta}) {
      for (auto y : {-delta, delta}) {
        n4::place(sipm_array)
          .rot_y(180 * deg)
          .at(x, y, -c.cath_thick/2 + c.sipm_thick)
          .in(cathode)
          .copy_no(n++)
          .name("far_plane")
          .now();
      }
    }
  }

  return n4::place(world).now();
}
