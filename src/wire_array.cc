#include "wire_array.hh"
#include "steel.hh"

#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"

#include <G4MultiUnion.hh>

#include <cmath>
#include <n4-geometry.hh>

G4LogicalVolume* create_wire_array(G4double frame_diam, G4double frame_thick, G4double frame_width, G4double pitch, G4double wire_diam) {
  auto frame = n4::tubs("frame")
    .r_inner(frame_diam/2)
    .r_delta(frame_width)
    .z(frame_thick);

  auto half_n_wires = std::floor(frame_diam / 2 / pitch);

  auto mesh = new G4MultiUnion();

  auto along_x_axis = G4RotationMatrix{}; along_x_axis.rotateY(90 * deg);

  for (auto i = 0; i < half_n_wires; i++) {
    for (auto s : {-1, 1}) {
      auto offset    = s * pitch * (i + 0.5);
      auto length    = 2 * std::sqrt(frame_diam*frame_diam/4 - offset*offset) + frame_width/2;
      auto wire      = n4::tubs("wire").r(wire_diam / 2).z(length).solid();
      auto pos       = G4ThreeVector{0,  offset, 0};
      auto transform = G4Transform3D(along_x_axis, pos);
      mesh -> AddNode(*wire, transform);
    }
  }

  mesh -> Voxelize();

  return frame.add(mesh).volume(steel_with_properties());
}
