#include "geometry/mesh.hh"
#include "materials/steel.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4LogicalVolume.hh>
#include <G4MultiUnion.hh>
#include <G4Polyhedra.hh>
#include <G4RotationMatrix.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4VPVParameterisation.hh>

#include <n4-geometry.hh>

#include <algorithm>
#include <utility>
#include <vector>

G4LogicalVolume* create_wire_mesh(G4double diam, G4double pitch, G4double wire_diam) {
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
  return n4::volume(fitted_mesh, steel_with_properties());
}

G4Polyhedra* hexagon(G4double circumradius, G4double thick) {
  G4double z_planes[2] = {-thick, thick};
  G4double  r_inner[2] = {0., 0.};
  G4double  r_outer[2] = {circumradius, circumradius};
  return new G4Polyhedra("hexahole", 0, CLHEP::twopi, 6, 2, z_planes, r_inner, r_outer);
}

typedef std::vector<std::pair<G4double, G4double>> posvec;

bool pair_in_vec(G4double x, G4double y, posvec &vec) {
  auto pair = std::make_pair(x, y);
  return std::find(vec.begin(), vec.end(), pair) != vec.end();
}

G4double round_to_8_digits(G4double x) {
  static auto scale = std::pow(10, 8);
  return std::round(x * scale) / scale;
}

void hexagon_positions(G4double x0, G4double y0, G4double pitch, G4double rmax2, posvec& existing, G4int nesting) {
  for (auto i=0; i<6; i++) {
    auto angle = (i + 0.5) * CLHEP::pi/3;
    auto x = round_to_8_digits(x0 + pitch * std::cos(angle));
    auto y = round_to_8_digits(y0 + pitch * std::sin(angle));

    if (x*x + y*y > rmax2)
      continue;

    if (pair_in_vec(x, y, existing))
      continue;

    existing.emplace_back(x, y);
    hexagon_positions(x, y, pitch, rmax2, existing, nesting + 1);
  }
}

posvec generate_hexagon_positions(G4double pitch, G4double rmax) {
  auto rmax2 = (rmax + pitch) * (rmax + pitch);

  std::vector<std::pair<G4double, G4double>> out;
  out.emplace_back(0, 0);
  hexagon_positions(0, 0, pitch, rmax2, out, 0);

  return out;
}

G4LogicalVolume* create_hex_mesh(G4double frame_diam, G4double frame_thick, G4double frame_width, G4double pitch, G4double mesh_thick, G4double hex_inradius) {
  auto protomesh = n4::tubs("frame")
    .r(frame_diam / 2 + frame_width)
    .z(frame_thick)
    .solid();

  auto frame = n4::tubs("frame")
    .r_inner(frame_diam / 2)
    .r_delta(frame_width)
    .z(frame_thick)
    .solid();

  auto hex_r = 2 / std::sqrt(3) * hex_inradius; // circumradius
  auto hex   = hexagon(hex_r, mesh_thick);

  // auto holes = new G4MultiUnion("holes");
  // for (auto p: generate_hexagon_positions(pitch, frame_diam/2)) {
  //   auto pos       = G4ThreeVector{p.first, p.second, 0};
  //   auto transform = G4Transform3D(along_x_axis, pos);
  //   holes -> AddNode(hex, transform);
  // }

  // holes -> Voxelize();

  // auto overperforated_mesh = new G4SubtractionSolid("trimmed_mesh", protomesh, holes);
  // auto         fitted_mesh = new G4UnionSolid      ( "fitted_mesh", frame    , overperforated_mesh);

  G4SubtractionSolid* mesh = nullptr;
  for (auto p: generate_hexagon_positions(pitch, frame_diam/2)) {
    auto pos = G4ThreeVector{p.first, p.second, 0};
    mesh = mesh ?
      new G4SubtractionSolid("mesh",      mesh, hex, nullptr, pos):
      new G4SubtractionSolid("mesh", protomesh, hex, nullptr, pos);
  }

  auto fitted_mesh = new G4UnionSolid( "fitted_mesh", frame, mesh);
  return n4::volume(fitted_mesh, steel_with_properties());
}
