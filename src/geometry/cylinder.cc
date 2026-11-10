#include "core/config.hh"
#include "core/symbols.hh"
#include "geometry/calibration_belt.hh"
#include "geometry/mesh.hh"
#include "geometry/sipm_array.hh"
#include "geometry/wire_array.hh"
#include "materials/LXe.hh"
#include "materials/LAr.hh"
#include "materials/LArXe.hh"
#include "materials/ptfe.hh"
#include "materials/steel.hh"
#include "sensitive/active.hh"


#include <G4Color.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4Polyhedra.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>

#include <cmath>

#include <n4-geometry.hh>
#include <n4-place.hh>
#include <n4-shape.hh>
#include <n4-volume.hh>
#include <n4-vis-attributes.hh>

#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4Transform3D.hh>
#include <G4SystemOfUnits.hh>
#include <G4ios.hh>

void PrintSiPMPositions(const G4VPhysicalVolume *pv,
                        const G4Transform3D &transformToWorld);


                        
G4Material* get_medium(Medium m, f64 xenon_fraction) {
  switch (m) {
  case Medium::XENON     : return LXe_with_properties(); //imported from LXe.hh
  case Medium::ARGON     : return LAr_with_properties(); //imported from LAr.hh
  case Medium::ARGONXENON: return LArXe_with_properties(xenon_fraction); //imported from LArXe.hh
  case Medium::KRYPTON   :
    G4Exception("[get_medium]", "", FatalException, "Krypton not implemented yet");
  };
  // unreachable, but C++ wants a receipt.
  return nullptr;
}


auto cylinder(const geometry_config& g){

  //Defining materials to be used in the geometry
  auto air   = n4::material("G4_AIR");
  auto medium = get_medium(g.medium, g.xenon_fraction);
  auto ptfe  =  ptfe_with_properties();
  auto steel = steel_with_properties();


  auto seethrough = [](G4Color c, double t=0.1) {
    return G4Color{c.GetRed(), c.GetGreen(), c.GetBlue(), t};
  };
  //Defining colors for visualization (no effect on the simulation)
  auto transparent = n4::vis_attributes().visible(true).colour({1, 1, 1, 0});
  auto invisible   = n4::vis_attributes().visible(false);
  // auto wireframe   = n4::vis_attributes().visible(true).color(G4Color::White()).force_wireframe(true).line_width(5);
  auto  white      = n4::vis_attributes().visible(true).color(G4Color::White());
  auto twhite      = n4::vis_attributes().visible(true).color(seethrough(G4Color::White()));
  auto green       = n4::vis_attributes().visible(true).color(G4Color::Green());
  auto tblue       = n4::vis_attributes().visible(true).color(seethrough(G4Color::Blue(), 0.3));
  auto gray        = n4::vis_attributes().visible(true).color(G4Color::Gray());
  auto tgray       = n4::vis_attributes().visible(true).color(seethrough(G4Color::Gray(), 0.3));
  auto tred        = n4::vis_attributes().visible(true).color(seethrough(G4Color::Red(), 0.05));


  auto world = n4::box("world")
    .xy(1.5 * g.vessel_outer )
    .z(1.5 * g.vessel_length)
    .vis(transparent)
    .volume(air);


  auto vessel = n4::tubs("vessel")
    .r_inner(g.vessel_inner / 2)
    .r(g.vessel_outer / 2)
    .z(g.vessel_length)
    .vis(tgray)
    .place(steel)
    .in(world)
    .now();



  auto liquid = n4::tubs("liquids")
      .r(g.vessel_inner / 2)
      .z(g.vessel_length)
      .vis(tblue)
      .place(medium)
      .in(world)
      .now();

  auto reflector = n4::tubs("reflector")
    .r_inner(g.drift_diam / 2)
    .r_delta(g.ptfe_thick)
    .z(g.drift_length)
    .vis(white)
    .place(ptfe)
    .in(liquid)
    .now();

    new G4LogicalSkinSurface("reflector_surface", reflector -> GetLogicalVolume(), ptfe_surface());

  // Field cage rings
  for (auto i=0; i<g.fc_rings; i++) {
      char name[30];
      auto z  = g.fc_ring_zpitch * i + g.fc_ring_thick / 2;
      
      std::sprintf(name, "field_cage_ring_%d", i);
      auto ring = n4::tubs(name)
      .r_inner(g.drift_diam / 2 + g.ptfe_thick)
      .r_delta(g.fc_ring_width)
      .z(g.fc_ring_thick)
      .vis(gray)
      .place(steel)
      .in(liquid)
      .at_z(-g.drift_length / 2 + z)
      .now();

      char name2[30];
      std::sprintf(name2, "%s_surface", name);
      new G4LogicalSkinSurface(name2, ring -> GetLogicalVolume(), steel_surface());
  }


  auto mesh_el = create_hex_mesh( g.drift_diam
                              , g.mesh_thick
                              , g.frame_width
                              , g.mesh_hex_pitch
                              , g.mesh_hex_inradius
                               // wire_top boolean (see mesh.cc)
                            );
  // mesh_el -> SetVisAttributes(invisible);
  
  
  //Half of the thickness of the mesh for gate because in mesh.cc we position the frame on the relative 0 with half of its thickness to each direction
  n4::place(mesh_el).name("gate").at_z(g.drift_length/2 + g.frame_thick_mesh   + g.mesh_thick/2                                           ).in(liquid).now();
  //Adding complete thickness. Half from the gate and half from the shield for the same reason. Extra half for the positioning of the gate mesh
  n4::place(mesh_el).name("shield").rot_y(180 * deg).at_z(g.drift_length/2 + g.frame_thick_mesh + g.d_gate_wire + g.d_wire_shield + g.mesh_thick/2).in(liquid).now();
  


  auto wire_array = create_wire_array(g); //defined in wire_array.cc

  wire_array -> SetVisAttributes(gray);
  n4::place(wire_array)
    .rot_z(g.thin_wire_rot)
    .at_z(g.drift_length/2 + g.frame_thick_mesh + g.mesh_thick/2 + g.d_gate_wire) //Adding full gate thickness because of the positioning of the mesh
    .in(liquid)
    .now();
  


  auto gate_holder = n4::tubs("gate_holder")
    .r_inner(g.drift_r())
    .r_delta(g.ptfe_thick)
    .z(g.frame_thick_mesh)
    .vis(green)
    .place(ptfe)
    .in(liquid)
    .at_z(g.drift_length/2 + g.frame_thick_mesh/2)
    .now();
  new G4LogicalSkinSurface("gate_holder_surface", gate_holder -> GetLogicalVolume(), ptfe_surface());

  auto spacer_gate_wire_thickness = g.d_gate_wire - g.frame_thick_wires/2 - g.mesh_thick/2;
    auto spacer_gate_wire = n4::tubs("spacer_gate_wire")
      .r_inner(g.drift_r())
      .r_delta(g.ptfe_thick)
      .z(spacer_gate_wire_thickness)
      .vis(white)
      .place(ptfe)
      .in(liquid)
      .at_z(g.drift_length/2 + g.frame_thick_mesh + g.mesh_thick + spacer_gate_wire_thickness/2 )
      .now();
    new G4LogicalSkinSurface("spacer_gate_wire_surface", spacer_gate_wire -> GetLogicalVolume(), ptfe_surface());




  auto spacer_wire_shield_thickness = g.d_wire_shield - g.frame_thick_wires/2 - g.mesh_thick/2;
    auto spacer_wire_shield = n4::tubs("spacer_wire_shield")
      .r_inner(g.drift_r())
      .r_delta(g.ptfe_thick)
      .z(spacer_wire_shield_thickness)
      .vis(white)
      .place(ptfe)
      .in(liquid)
      .at_z(g.drift_length/2 + g.frame_thick_mesh + g.mesh_thick/2 + g.d_gate_wire + g.frame_thick_wires/2 + spacer_wire_shield_thickness/2)
      .now();
    new G4LogicalSkinSurface("spacer_wire_shield_surface", spacer_wire_shield -> GetLogicalVolume(), ptfe_surface());

  auto shield_holder = n4::tubs("shield_holder")
    .r_inner(g.drift_r())
    .r_delta(g.ptfe_thick)
    .z(g.frame_thick_mesh)
    .vis(green)
    .place(ptfe)
    .in(liquid)
    .at_z(g.drift_length/2 + 3 * g.frame_thick_mesh/2 + g.d_gate_wire + g.d_wire_shield + g.mesh_thick)
    .now();
  new G4LogicalSkinSurface("shield_holder_surface", shield_holder -> GetLogicalVolume(), ptfe_surface());  


  auto z_cathode = - g.drift_length/2;
  z_cathode -= g.ptfe_on_fp ? g.ptfe_thick : 0;
  z_cathode -= (g.sipms_on_fp
              ? (g.ptfe_on_fp ? g.sipm_thick : 2 * g.sipm_thick)
              : 0);

 


 auto cathode_mesh = create_wire_mesh(g.cath_diam_cylinder(), g.wire_mesh_pitch, g.thin_wire_diam);
 cathode_mesh -> SetVisAttributes(gray);


 //G4PVPlacement* ptfe_below_cathode = nullptr;
 auto z_ptfe_below_cathode = - g.drift_length/2 - g.ptfe_thick/2 - 2 * g.thin_wire_diam;
 z_ptfe_below_cathode -= g.sipms_on_fp ? 2 * g.sipm_thick : 0;
 
 auto ptfe_below_cathode = n4::tubs("ptfe_below_cathode")
   .r(g.cath_r_cylinder() + g.ptfe_thick)
   .z(g.ptfe_thick)
   .vis(white)
   .place(ptfe);
   


  


  auto cathode = n4::tubs("cathode")
    .r(g.cath_r_cylinder() + g.ptfe_thick)
    .z(g.cath_thick)
    .vis(gray)
    .place(steel)
    ;

  
  

  G4PVPlacement* ptfe_cathode = nullptr;
  auto z_ptfe_cathode = - g.drift_length/2 - g.ptfe_thick/2;
  z_ptfe_cathode -= g.sipms_on_fp ? 2 * g.sipm_thick : 0;
  if (g.ptfe_on_fp) {
    ptfe_cathode = n4::tubs("ptfe_cathode")
      .r(g.cath_r_cylinder() + g.ptfe_thick)
      .z(g.ptfe_thick)
      .vis(white)
      .place(ptfe)
      .at_z(z_ptfe_cathode)
      .in(liquid)
      .now();

    new G4LogicalSkinSurface("cath_surface", ptfe_cathode -> GetLogicalVolume(), ptfe_surface());
  }

  if (g.mesh_cathode) {
            n4::place(cathode_mesh).name("mesh_cathode").at_z(z_cathode - g.thin_wire_diam / 2).in(liquid).now();
            n4::place(ptfe_below_cathode).at_z(z_ptfe_below_cathode)
                                         .in(liquid)
                                         .now();
             new G4LogicalSkinSurface("cath_surface", ptfe_below_cathode.get_logical(), ptfe_surface());
  }   else{
            n4::place(cathode).at_z(z_cathode - g.cath_thick / 2)
                              .in(liquid)
                              .now();
            new G4LogicalSkinSurface("cathode_surface", cathode.get_logical(), steel_surface());
  }


  
  auto sipm_array = build_sipm_array(g.sipm_size, g.sipm_thick, g.sipm_gap, g.n_sipm_side);
  n4::place(sipm_array)
    .at_z(g.drift_length/2 + 2 * g.frame_thick_mesh  + g.d_gate_wire + g.d_wire_shield + g.mesh_thick + g.d_shield_sipms + g.sipm_thick)
    .in(liquid)
    .copy_no(0)
    .name("near_plane")
    .now();

  //sipm_array -> SetVisAttributes(invisible); //to see the cathode
  //auto sipm_hexagon = build_sipm_array_hexagonal(g.sipm_size, g.sipm_thick, g.drift_r() * 0.75, g.drift_r());
  //n4::place(sipm_hexagon)
    //.at_z(g.drift_length + 2 * g.frame_thick_mesh  + g.d_gate_wire + g.d_wire_shield + g.d_shield_sipms + g.sipm_thick)
    //.in(liquid)
    //.copy_no(0)
    //.name("near_plane_2")
    //.now();
  


  if (g.sipms_on_fp) {
    
    auto z =  g.ptfe_on_fp                  ?
                  g.ptfe_thick/2 + g.sipm_thick :
                  g.cath_thick/2 + g.sipm_thick ;
    n4::place(sipm_array)
      .rot_y(180 * deg) //We do the mirror image of the other array
      .at(0, 0, z)
      .in(g.ptfe_on_fp ? ptfe_cathode -> GetLogicalVolume() : cathode.get_logical())
      .name("far_plane")
      .now();
  }
  

  // return n4::place(world).now();
  auto full_geometry = n4::place(world).now();
  PrintSiPMPositions(full_geometry, G4Transform3D());
  return full_geometry;
}

//We define the function here temporarily
void PrintSiPMPositions(const G4VPhysicalVolume* pv, const G4Transform3D& transformToWorld)
{
    // Position of this volume's local origin in world coordinates
    const auto pos = transformToWorld.getTranslation();

    if (pv -> GetName().rfind("sipm_", 0) == 0) {
        std::cout << pv -> GetName()
                  << " copy " << pv -> GetCopyNo()
                  << " : ("
                  << pos.x()/mm << ", "
                  << pos.y()/mm << ", "
                  << pos.z()/mm << ") mm"
                  << std::endl;
    }

    auto lv = pv -> GetLogicalVolume();
    for (G4int i = 0; i < lv -> GetNoDaughters(); ++i) { //iteration over all the objects in the physical volume
        auto* daughter = lv -> GetDaughter(i);

        const G4Transform3D daughterToMother( daughter -> GetObjectRotationValue(),
                                              daughter -> GetObjectTranslation());

        const auto daughterToWorld = transformToWorld * daughterToMother;

        PrintSiPMPositions(daughter, daughterToWorld);
    }
}





