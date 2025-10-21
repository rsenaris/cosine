#include "geometry/mesh.hh"

#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"
#include <G4MultiUnion.hh>

#include <n4-geometry.hh>

G4LogicalVolume* create_mesh(G4double diam, G4double pitch, G4double wire_diam) {
  auto envelope = n4::tubs("mesh_envelope")
    .r_inner(diam/2)
    .r(diam)
    .z(2 * wire_diam)
    .solid();
  auto wire = n4::tubs("wire").r(wire_diam / 2).z(diam).solid();

  auto half_n_wires = std::floor(diam / 2 / pitch);

  auto mesh = new G4MultiUnion();

  auto along_x_axis = G4RotationMatrix{}; along_x_axis.rotateY(90 * deg);
  auto along_y_axis = G4RotationMatrix{}; along_y_axis.rotateX(90 * deg); along_y_axis.rotateY(90 * deg);

  for (auto i = 0; i < half_n_wires; i++) {
    for (auto s : {-1, 1} ) {
      auto pos       = G4ThreeVector{0,  s * pitch * (i + 0.5), 0};
      auto transform = G4Transform3D(along_x_axis, pos);
      mesh -> AddNode(*wire, transform);
    }
  }

  for (auto i = 0; i < half_n_wires; i++) {
    for (auto s : {-1, 1} ) {
      auto pos       = G4ThreeVector{0,  s * pitch * (i + 0.5), 0};
      auto transform = G4Transform3D(along_y_axis, pos);
      mesh -> AddNode(*wire, transform);
    }
  }

  mesh -> Voxelize();

  G4SubtractionSolid* fitted_mesh = new G4SubtractionSolid("fitted_mesh", mesh, envelope);
  auto steel = n4::material("G4_STAINLESS-STEEL");
  return n4::volume(fitted_mesh, steel);
}
