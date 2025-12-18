#include "geometry/calibration_belt.hh"
#include "config.hh"
#include "materials/steel.hh"

#include <n4-vis-attributes.hh>

#include <G4PVPlacement.hh>

#include <cmath>

G4PVPlacement* straight_calibration_belt(const geometry_config& g, G4PVPlacement* liquid, n4::vis_attributes& attrs) {
    auto r = g.el_r()
           + g.form_factor * (g.drift_length / 2 + g.fc_ring_width / 2)
           + g.fc_ring_thick
           + g.calib_belt_separation
           + g.calib_belt_router;

    auto tilt = std::atan(g.form_factor);

    auto steel = steel_with_properties();

    auto length = std::sqrt(1 + g.form_factor * g.form_factor) * g.drift_length;
    return n4::tubs("calibration_belt")
      .r_inner(g.calib_belt_rinner)
      .r_delta(g.calib_belt_router)
      .z(length)
      .vis(attrs)
      .place(steel)
      .in(liquid)
      .rotate_y(tilt)
      .at({r, 0, g.neck_length + g.drift_length / 2})
      .now();
}
