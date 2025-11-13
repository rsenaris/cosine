#pragma once

#include "types.hh"
#include "utils.hh"

#include <G4SystemOfUnits.hh>

#include <string>


struct sim_config {
  std::string generator;
  u32 nparticles;
  u64 seed;

  static sim_config s1();
  static sim_config s2();
};

inline sim_config sim_config::s1() {
  return {.generator="s1", .nparticles=intpow((u32) 10, 7), .seed=1234567890};
}

inline sim_config sim_config::s2() {
  return {.generator="s2", .nparticles=intpow((u32) 10, 6), .seed=1234567890};
}

struct geometry_config {
  f64 el_diam;
  f64 drift_length;
  f64 form_factor;
  f64 cath_diam;
  f64 wall_thick;
  f64 d_gate_wire;
  f64 d_wire_shield;
  f64 d_shield_sipms;
  f64 mesh_hex_pitch;
  f64 mesh_thick;
  f64 mesh_hex_inradius;
  f64 thin_wire_pitch;
  f64 thin_wire_diam;
  f64 thin_wire_rot;
  f64 sipm_size;
  f64 sipm_thick;
  f64 sipm_gap;
  u16 n_sipm_side;
  f64 cath_thick;
  f64 frame_thick_mesh;
  f64 frame_thick_wires;
  f64 frame_width;
  f64 neck_length;
  u16 fc_rings;
  f64 fc_ring_pitch;
  f64 fc_ring_width;
  f64 fc_ring_thick;

  f64   el_r;
  f64 cath_r;

  bool sipms_on_fp;
  bool  ptfe_on_fp;
  bool ptfe_on_walls;

  const std::vector<f64>& wire_poss() const;
  const std::vector<f64>& wire_lengths() const;

  static geometry_config pcolina();
  static geometry_config colina();
};

inline geometry_config geometry_config::colina() {
  auto form_factor  = 1.0; // ratio between drift length and cathode radius
  auto drift_length = 240 * mm;
  auto el_diam      = 64 * mm;
  auto cath_diam    = 2 * form_factor * drift_length + el_diam;
  auto sipm_thick   = 1 * mm;
  auto wall_thick   = 1 * mm;

  return {
  .el_diam               = el_diam,
  .drift_length          = drift_length,
  .form_factor           = form_factor,
  .cath_diam             = cath_diam,
  .wall_thick            = wall_thick,
  .d_gate_wire           = 5    * mm,
  .d_wire_shield         = 5    * mm,
  .d_shield_sipms        = 3    * mm,
  .mesh_hex_pitch        = 10.2 * mm,
  .mesh_thick            = 0.2  * mm,
  .mesh_hex_inradius     = 10.0 * mm,
  .thin_wire_pitch       = 5    * mm,
  .thin_wire_diam        = 10   * um, // Use 0.1 * mm for visualization
  .thin_wire_rot         = 45   * deg,
  .sipm_size             = 6    * mm,
  .sipm_thick            = sipm_thick,
  .sipm_gap              = 0.5  * mm,
  .n_sipm_side           = 10,        // array of NxN
  .cath_thick            = 3 * sipm_thick,
  .frame_thick_mesh      = 2 * mm,
  .frame_thick_wires     = 4 * mm,
  .frame_width           = wall_thick,
  .     neck_length      = 10 * mm,
  .fc_rings              = 8,
  .fc_ring_pitch         = 30 * mm,
  .fc_ring_width         =  5 * mm,
  .fc_ring_thick         =  2 * mm,

  .el_r   = el_diam / 2.0,
  .cath_r = cath_diam / 2,

  .sipms_on_fp   = false,
  . ptfe_on_fp   = true,
  .ptfe_on_walls = true,
  };
}
