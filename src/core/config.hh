#pragma once

#include "core/types.hh"
#include "core/symbols.hh"

#include <G4ThreeVector.hh>
#include <G4SystemOfUnits.hh>

#include <optional>
#include <string>
#include <vector>

extern u64 START_ID;

struct sim_config {
  std::string outputfile = "output_file.h5"; //file where we save the config and the results
  EventGenerator generator;

  u32 nparticles;
  u64 seed = 1234567890;
  u64 start_id = START_ID;

  bool optical = false;

  bool store_steps   = false;
  bool store_sens    = true;
  bool store_sources = true;
  bool store_ihits   = true;
  bool store_tracks  = true;

  static sim_config empty();
  static sim_config cylinder_geometry_probe();

  std::optional<G4ThreeVector> vertex = std::nullopt;

  /// All independent class members must be added to the parameter_list
  std::vector<ConfPar> parameter_list();
};

inline sim_config sim_config::empty() {
  return {.generator=EventGenerator::S1, .nparticles=0};
}

inline sim_config sim_config::cylinder_geometry_probe() {
  return {.generator=EventGenerator::G0, .nparticles=1};
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
  f64 sipm_side_gap;
  u16 n_sipm_side;
  u16 n_blocks_side;
  f64 sipm_block_gap;
  f64 sipm_block_size;
  f64 sipm_block_thick;
  f64 cath_thick;
  f64 frame_thick_mesh;
  f64 frame_thick_wires;
  f64 frame_width;
  f64 neck_length;
  u16 fc_rings;
  f64 fc_ring_zpitch;
  f64 fc_ring_width;
  f64 fc_ring_thick;
  f64 pillow_plate_thick;
  f64 cryostat_wall_thick;
  f64 d_ptfe_cryostat;


  //params for cylinder
  f64 vessel_inner;
  f64 vessel_length;
  f64 vessel_outer;
  f64 drift_diam;
  f64 ptfe_thick;
  f64 wire_mesh_pitch;
  
  


  CalibrationBelt calib_belt;
  f64 calib_belt_router;
  f64 calib_belt_rinner;
  f64 calib_belt_separation;
  f64 calib_belt_loops;

  Medium medium;
  f64 xenon_fraction = 0.1;

  bool sipms_on_fp;
  bool  ptfe_on_fp;
  bool ptfe_on_walls;
  bool mesh_cathode;

  f64 cath_diam() const { return 2 * form_factor * drift_length + el_diam; }
  f64 cath_r   () const { return cath_diam()/2; }
  f64 el_r     () const { return el_diam/2; }
  f64 drift_r  () const { return drift_diam/2; }
  f64 cath_diam_cylinder() const {return 1.5 * drift_diam; }
  f64 cath_r_cylinder   () const { return cath_diam_cylinder()/2; }

  void check() const;

  const std::vector<f64>& wire_poss() const;
  const std::vector<f64>& wire_lengths() const;

  static geometry_config pcolina();
  static geometry_config colina();
  static geometry_config cylinder();

  /// All independent class members must be added to the parameter_list
  std::vector<ConfPar> parameter_list();
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
  .sipm_gap              = 0.5  * mm,
  .sipm_side_gap         = 0.2  * mm, 
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
  .pillow_plate_thick    =  1 * mm,
  .cryostat_wall_thick   =  3 * mm,
  .d_ptfe_cryostat       =  4 * mm,

  .calib_belt_router     = 5 * mm,
  .calib_belt_rinner     = 4 * mm,
  .calib_belt_separation = 3 * mm,
  .calib_belt_loops      = 2,

  .medium = Medium::XENON,
  .xenon_fraction = 0.0,

  .sipms_on_fp   = true,
  . ptfe_on_fp   = true,
  .ptfe_on_walls = true,
  };
}



inline geometry_config geometry_config::cylinder() {
  return {
  .drift_length          = 120 * mm,
  .form_factor           = 1.0,
  //.wall_thick            = 1.0  * mm,
  .d_gate_wire           = 5    * mm,
  .d_wire_shield         = 5    * mm,
  .d_shield_sipms        = 2    * mm,
  .mesh_hex_pitch        = 10.15 * mm,
  .mesh_thick            = 0.2  * mm,//antes eran mm
  .mesh_hex_inradius     = 2 * mm, //It seems like the limit for visualization is 1/4 of the pitch (2.5 mm)
  .thin_wire_pitch       = 5    * mm,
  .thin_wire_diam        = 10   * um, // Use 0.1 * mm for visualization
  .thin_wire_rot         = 45   * deg,
  .sipm_size             = 6    * mm,
  .sipm_thick            = 1    * mm,
  .sipm_gap              = 0.5  * mm,
  .sipm_side_gap         = 0.2  * mm,
  .n_sipm_side           = 10,        // array of NxN
  .n_blocks_side         = 5,           //array os 2NX2N (each block has 4 SIPMs. See sipm_array.cc)
  .sipm_block_gap        = 0.2  * mm,
  .sipm_block_size       = 15   * mm, 
  .sipm_block_thick      = 2.5  * mm, 
  .cath_thick            = 5 * mm,
  .frame_thick_mesh      = 3 * mm,
  .frame_thick_wires     = 6 * mm,//Width of the frame that holds the anode wires 
  .frame_width           = 1 * mm,  
  .fc_rings              = 12,
  .fc_ring_zpitch        = 10 * mm,
  .fc_ring_width         =  5 * mm,
  .fc_ring_thick         =  2 * mm,
  .vessel_inner          = 127 * mm,
  .vessel_length         = 230 * mm,
  .vessel_outer          = 154 * mm,
  .drift_diam            = 64 * mm,
  .ptfe_thick            = 1.0 * mm,
  .wire_mesh_pitch       = 5. * mm,
  

  .medium = Medium::XENON,
  .xenon_fraction = 0.0, //in ppm


  .sipms_on_fp   = false,
  .ptfe_on_fp    = false,
  .mesh_cathode  = false,
  
  
  };
}