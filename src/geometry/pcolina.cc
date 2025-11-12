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

auto pcolina() {
  auto el_diam               = 64   * mm;
  auto el_r                  = el_diam / 2.0;
  auto drift_length          = 240  * mm;
  auto form_factor           = 1.0; // ratio between drift length and cathode radius
  auto cath_diam             = 2 * form_factor * drift_length + el_diam;
  auto cath_r                = cath_diam / 2.0;
  auto wall_thick            = 1    * mm;
  auto d_gate_wire           = 5    * mm;
  auto d_wire_shield         = 5    * mm;
  auto d_shield_sipms        = 3    * mm;
  auto mesh_hex_pitch        = 10.2 * mm;
  auto mesh_thick            = 0.2  * mm;
  auto mesh_hex_inradius     = 10.0 * mm;
  auto thin_wire_pitch       = 5    * mm;
  auto thin_wire_diam        = 10   * um; // Use 0.1 * mm for visualization
  auto thin_wire_rot         = 45   * deg;
  auto sipm_size             = 6    * mm;
  auto sipm_thick            = 1    * mm;
  auto sipm_gap              = 0.5  * mm;
  auto n_sipm_side           = 10;        // array of NxN
  auto cath_thick            = 3 * sipm_thick;
  auto frame_thick_mesh      = 2 * mm;
  auto frame_thick_wires     = 4 * mm;
  auto frame_width           = wall_thick;
  auto      neck_length      = 10 * mm;
  auto full_neck_length      = neck_length
                             + mesh_thick // gate thickness
                             + d_gate_wire   - frame_thick_wires/2
                             + d_wire_shield - frame_thick_wires/2
                             + mesh_thick // shield thickness
                             + d_shield_sipms;
  auto fc_rings              = 8;
  auto fc_ring_pitch         = 30 * mm;
  auto fc_ring_width         =  5 * mm;
  auto fc_ring_thick         =  2 * mm;

  auto sipms_on_fp    = false;
  auto  ptfe_on_fp    = true;
  auto  ptfe_on_walls = true;

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
    .cube(1.2 * cath_diam)
    .z(2.5 * drift_length)
    .vis(frame)
    .volume(air);

  auto liquid_length = (drift_length + full_neck_length) * 2.1;
  auto liquid = n4::box("liquid")
    .cube(1.1 * cath_diam)
    .z(liquid_length)
    .vis(red)
    .place(lxe)
    .in(world)
    .now();

  auto neck = n4::tubs("neck")
    .r_inner(el_r)
    .r_delta(wall_thick)
    .z(neck_length)
    .vis(white)
    .place(ptfe)
    .in(liquid)
    .at_z(neck_length/2)
    .now();

  new G4LogicalSkinSurface("neck_surface", neck -> GetLogicalVolume(), ptfe_surface());


  // Field cage rings
  for (auto i=0; i<fc_rings; i++) {
    char name[20];
    auto z  = fc_ring_pitch*i + fc_ring_width/2;
    auto r1 = el_r + form_factor * (z - fc_ring_width/2);
    auto r2 = el_r + form_factor * (z + fc_ring_width/2);
    std::sprintf(name, "field_cage_ring_%d", i);
    auto ring = n4::cons(name)
      .r1_inner(r1)
      .r2_inner(r2)
      .r_delta(fc_ring_thick)
      .z(fc_ring_width)
      .vis(gray)
      .place(steel)
      .in(liquid)
      .at_z(neck_length + z)
      .now();

    char name2[30];
    std::sprintf(name2, "%s_surface", name);
    new G4LogicalSkinSurface(name2, ring -> GetLogicalVolume(), steel_surface());
  }

  if (ptfe_on_walls) {
    auto ptfe_walls = n4::cons("ptfe_walls")
      .r1_inner(el_r)
      .r2_inner(cath_r)
      .r_delta(wall_thick)
      .z(drift_length)
      .vis(wireframe)
      .place(ptfe)
      .in(liquid)
      .at_z(neck_length + drift_length/2)
      .now();

    new G4LogicalSkinSurface("cath_surface", ptfe_walls -> GetLogicalVolume(), ptfe_surface());
  }

  auto wire_array = create_wire_array( el_diam
                                     , frame_thick_wires
                                     , wall_thick
                                     , thin_wire_pitch
                                     , thin_wire_diam);
  wire_array -> SetVisAttributes(gray);
  n4::place(wire_array)
    .rot_z(thin_wire_rot)
    .at_z(-mesh_thick - d_gate_wire)
    .in(liquid)
    .now();

  auto mesh_el = create_hex_mesh( el_diam
                                , frame_thick_mesh
                                , frame_width
                                , mesh_hex_pitch
                                , mesh_thick
                                , mesh_hex_inradius);
  mesh_el -> SetVisAttributes(invisible);
  // mesh_el -> SetVisAttributes(green);

  n4::place(mesh_el).name("gate"  ).at_z(-mesh_thick/2                                        ).in(liquid).now();
  n4::place(mesh_el).name("shield").at_z(-mesh_thick -d_gate_wire -d_wire_shield -mesh_thick/2).in(liquid).now();

  auto z_cathode = neck_length + drift_length + cath_thick/2;
  z_cathode += ptfe_on_fp ? wall_thick : 0;
  auto cathode = n4::tubs("cathode")
    .r(cath_r + wall_thick)
    .z(cath_thick)
    .vis(gray)
    .place(steel)
    .at_z(z_cathode)
    .in(liquid)
    .now();

  new G4LogicalSkinSurface("cathode_surface", cathode -> GetLogicalVolume(), steel_surface());

  if (ptfe_on_fp) {
    auto ptfe_cathode = n4::tubs("ptfe_cathode")
      .r(cath_r + wall_thick)
      .z(wall_thick)
      .vis(white)
      .place(ptfe)
      .at_z(z_cathode -cath_thick/2 -wall_thick/2)
      .in(liquid)
      .now();

    new G4LogicalSkinSurface("cath_surface", ptfe_cathode -> GetLogicalVolume(), ptfe_surface());
  }

  auto sipm_array = build_sipm_array(sipm_size, sipm_thick, sipm_gap, n_sipm_side);
  n4::place(sipm_array)
    .at_z(-full_neck_length -sipm_thick)
    .in(liquid)
    .copy_no(0)
    .name("near_plane")
    .now();

  if (sipms_on_fp) {
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
  }

  return n4::place(world).now();
}
