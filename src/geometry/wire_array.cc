#include "config.hh"
#include "geometry/wire_array.hh"
#include "materials/steel.hh"
#include "utils.hh"

#include <G4LogicalSkinSurface.hh>
#include <G4LogicalVolume.hh>
#include <G4RotationMatrix.hh>
#include <G4MultiUnion.hh>

#include <n4-geometry.hh>

#include <cmath>

G4LogicalVolume *create_wire_array(const geometry_config &g) {
  auto frame = n4::tubs("frame")
    .r_inner(g.el_r)
    .r_delta(g.frame_width)
    .z(g.frame_thick_wires);

  auto mesh = new G4MultiUnion();

  auto along_x_axis = G4RotationMatrix{}; along_x_axis.rotateY(90 * deg);

  for (const auto &[p, l] : zip(g.wire_poss(), g.wire_lengths())) {
    auto wire = n4::tubs("wire")
      .r(g.thin_wire_diam / 2)
      .z(l + g.frame_width / 2) // overextend the wire to make sure it overlaps with frame
      .solid();
    auto pos       = G4ThreeVector{0, p, 0};
    auto transform = G4Transform3D(along_x_axis, pos);
    mesh -> AddNode(*wire, transform);
  }

  mesh -> Voxelize();

  auto wire_array = frame.add(mesh).volume(steel_with_properties());

  new G4LogicalSkinSurface("wires_surface", wire_array, steel_surface());
  return wire_array;
}
