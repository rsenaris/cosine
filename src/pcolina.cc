#include <G4SystemOfUnits.hh>
#include "G4Color.hh"
#include "G4Colour.hh"
#include "LXe.hh"
#include "ptfe.hh"

#include <n4-geometry.hh>
#include <n4-vis-attributes.hh>

auto pcolina() {
  auto el_diam      = 32 * mm;
  auto el_r         = el_diam / 2.0;
  auto drift_length = 120 * mm;
  auto form_factor  = 1.0; // ratio between drift length and cathode radius
  auto cath_diam    = form_factor * drift_length + el_diam;
  auto cath_r       = cath_diam / 2.0;
  auto wall_thick   = 1 * mm;
  auto air    = n4::material("G4_AIR");
  auto lxe    = LXe_with_properties();
  auto ptfe   = ptfe_with_properties();
  auto world_size = 1.1 * cath_diam;

  auto tred = G4Colour {1, 0 ,0, 0.5 };
  auto white = n4::vis_attributes().visible(true).color(G4Color::White());
  auto red   = n4::vis_attributes().visible(true).color(tred);
  auto frame = n4::vis_attributes().visible(true).color(G4Color::Grey ()).force_wireframe(true);

  auto world = n4::box("world")
    .cube(world_size)
    .vis(frame)
    .volume(air);

  n4::cons("walls")
    .r1_inner(el_r)
    .r2_inner(cath_r)
    .r_delta(wall_thick)
    .z(drift_length)
    .vis(white)
    .place(ptfe)
    .in(world)
    .now()
    ;

  n4::cons("active")
    .r1(el_r)
    .r2(cath_r)
    .z(drift_length)
    .vis(red)
    .place(lxe)
    .in(world)
    .now();



  return n4::place(world).now();
}
