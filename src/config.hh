#pragma once

#include "types.hh"
#include "utils.hh"

#include <G4SystemOfUnits.hh>

#include <string>

extern u64 START_ID;

struct sim_config {
  std::string outputfile = "output_file.h5";
  std::string generator;
  u32 nparticles;
  u64 seed = 1234567890;
  u64 start_id = START_ID;

  bool store_steps        = false;
  bool store_sens         = true;
  bool store_interactions = true;

  static sim_config s1();
  static sim_config s2();
};

inline sim_config sim_config::s1() {
  return {.generator="s1", .nparticles=330};
}

inline sim_config sim_config::s2() {
  return {.generator="s2", .nparticles=11000};
}

struct geometry_config {
  f64 el_diam;
  f64 drift_length;
  f64 form_factor;
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
  f64 fc_ring_zpitch;
  f64 fc_ring_width;
  f64 fc_ring_thick;

  bool sipms_on_fp;
  bool  ptfe_on_fp;
  bool ptfe_on_walls;

  f64 cath_diam() const { return 2 * form_factor * drift_length + el_diam; }
  f64 cath_r   () const { return cath_diam()/2; }
  f64 el_r     () const { return el_diam/2; }

  void check() const;

  const std::vector<f64>& wire_poss() const;
  const std::vector<f64>& wire_lengths() const;

  static geometry_config pcolina();
  static geometry_config colina();
};

inline geometry_config geometry_config::colina() {
  return {
  .el_diam               =  64  * mm,
  .drift_length          = 240  * mm,
  .form_factor           = 1.0,
  .wall_thick            = 1.0  * mm,
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
  .sipm_thick            = 1    * mm,
  .sipm_gap              = 0.2  * mm,
  .n_sipm_side           = 10,        // array of NxN
  .cath_thick            = 5 * mm,
  .frame_thick_mesh      = 2 * mm,
  .frame_thick_wires     = 4 * mm,
  .frame_width           = 1 * mm,
  .neck_length           = 10 * mm,
  .fc_rings              = 8,
  .fc_ring_zpitch        = 30 * mm,
  .fc_ring_width         =  5 * mm,
  .fc_ring_thick         =  2 * mm,

  .sipms_on_fp   = false,
  . ptfe_on_fp   = true,
  .ptfe_on_walls = true,
  };
}
