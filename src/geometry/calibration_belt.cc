#include "geometry/calibration_belt.hh"
#include "config.hh"
#include "materials/steel.hh"

#include <n4-vis-attributes.hh>
#include <n4-volume.hh>

#include <G4ExtrudedSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>

#include <cmath>

G4PVPlacement* straight_calibration_belt(const geometry_config& g, G4PVPlacement* liquid, n4::vis_attributes& attrs) {
    auto tilt   = std::atan(g.form_factor);
    auto steel  = steel_with_properties();
    auto length = std::sqrt(1 + g.form_factor*g.form_factor) * g.drift_length;

    // distance between wall and belt's axis
    auto d = g.calib_belt_separation + g.calib_belt_router;
    auto r = g.el_r()
           + g.wall_thick
           + g.d_ptfe_cryostat
           + g.cryostat_wall_thick
           + d * std::cos(tilt)
           + length/2 * std::sin(tilt);
    auto z = g.neck_length
           - d*std::sin(tilt)
           + length/2 * std::cos(tilt);

    return n4::tubs("calibration_belt")
      .r_inner(g.calib_belt_rinner)
      .r      (g.calib_belt_router)
      .z(length)
      .vis(attrs)
      .place(steel)
      .in(liquid)
      .rotate_y(tilt)
      .at({r, 0, z})
      .now();
}

G4ExtrudedSolid* spiral(f64 r_tube, const geometry_config& g, const std::string& name) {
  auto r0_spiral = g.el_r()
                 + g.wall_thick
                 + g.d_ptfe_cryostat
                 + g.cryostat_wall_thick
                 + g.calib_belt_separation
                 + g.calib_belt_router;

  std::vector<G4TwoVector> polygon = {};
  constexpr auto N = 10.0;
  for (auto i = 1.0; i <= N; i++) {
    auto a = (1 - i / N) * 360 * deg;
    polygon.emplace_back(r_tube * std::cos(a), r_tube * std::sin(a));
  }

  constexpr auto M = 1000.0;
  std::vector<G4ExtrudedSolid::ZSection> zsections{};
  for (auto i = 0.; i < M; i++) {
    auto z = i/M * g.drift_length;
    auto r = r0_spiral + g.form_factor * z;
    auto a = i/M * g.calib_belt_loops * CLHEP::twopi;
    auto x = r * std::cos(a);
    auto y = r * std::sin(a);
    zsections.push_back({z + g.neck_length, {x,y}, 1.});
  }

  return new G4ExtrudedSolid(name, polygon, zsections);
}

G4PVPlacement* spiral_calibration_belt(const geometry_config& g, G4PVPlacement* liquid, n4::vis_attributes& attrs) {
  auto steel = steel_with_properties();

  auto outer = spiral(g.calib_belt_router, g, "calib_belt_outer");
  auto inner = spiral(g.calib_belt_rinner, g, "calib_belt_inner");
  auto diff  = new G4SubtractionSolid("calibration_belt", outer, inner);
  auto logic = n4::volume(diff, steel); logic -> SetVisAttributes(attrs);
  return n4::place(logic)
    .in(liquid)
    .now();
}
