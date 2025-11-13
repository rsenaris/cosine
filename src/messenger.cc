#include "config.hh"
#include "messenger.hh"

#include "G4GenericMessenger.hh"
#include "Randomize.hh"

#include <memory>

messenger::messenger(sim_config &s, geometry_config &g)
    : s_(s), g_(g), msg_(nullptr)
{
  msg_ = std::make_unique<G4GenericMessenger>(this, "/sim/", "Simulation parameters");

  msg_ -> DeclareMethod  ("seed", &messenger::set_seed, "Set the random seed");
  msg_ -> DeclareProperty("nparticles", s_.nparticles);

#define SET(VAR) msg_-> DeclareProperty("VAR", g_.VAR)
  SET(el_diam);
  SET(drift_length);
  SET(form_factor);
  SET(wall_thick);
  SET(d_gate_wire);
  SET(d_wire_shield);
  SET(d_shield_sipms);
  SET(mesh_hex_pitch);
  SET(mesh_thick);
  SET(mesh_hex_inradius);
  SET(thin_wire_pitch);
  SET(thin_wire_diam);
  SET(thin_wire_rot);
  SET(sipm_size);
  SET(sipm_thick);
  SET(sipm_gap);
  SET(n_sipm_side);
  SET(cath_thick);
  SET(frame_thick_mesh);
  SET(frame_thick_wires);
  SET(frame_width);
  SET(neck_length);
  SET(fc_rings);
  SET(fc_ring_zpitch);
  SET(fc_ring_width);
  SET(fc_ring_thick);
  SET(sipms_on_fp);
  SET( ptfe_on_fp);
  SET(ptfe_on_walls);
#undef SET

  set_seed(s_.seed);
}

void messenger::set_seed(u64 s) {
  s_.seed = s;
  G4Random::setTheSeed(s_.seed);
}
