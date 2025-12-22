#include "config.hh"
#include "messenger.hh"
#include "types.hh"

#include <G4Exception.hh>
#include <G4GenericMessenger.hh>
#include <Randomize.hh>

#include <memory>
#include <string>

messenger::messenger(sim_config& s, geometry_config& g)
    : s_(s), g_(g), msg_(nullptr)
{
  msg_ = std::make_unique<G4GenericMessenger>(this, "/sim/", "Simulation parameters");

  // This one is better implemented as a method because there is no nice way of
  // inserting it in the flow
  msg_ -> DeclareMethod  ("seed"      , &messenger::set_seed      , "Set the random seed");
  msg_ -> DeclareMethod  ("start_id"  , &messenger::set_start_id  , "Set the random seed");
  msg_ -> DeclareMethod  ("calib_belt", &messenger::set_calib_belt, "Set the calibration belt shape");

#define  SET(VAR      ) msg_-> DeclareProperty(#VAR, s_.VAR)
  SET(outputfile);
  SET(generator);
  SET(nparticles);
  SET(store_steps);
  SET(store_sens);
  SET(store_sources);
#undef SET

#define  SET(VAR      ) msg_-> DeclareProperty(#VAR, g_.VAR)
#define SETU(VAR, UNIT) msg_-> DeclarePropertyWithUnit(#VAR, #UNIT, g_.VAR)
  SETU(el_diam          ,  mm);
  SETU(drift_length     ,  mm);
  SET (form_factor           );
  SETU(wall_thick       ,  mm);
  SETU(d_gate_wire      ,  mm);
  SETU(d_wire_shield    ,  mm);
  SETU(d_shield_sipms   ,  mm);
  SETU(mesh_hex_pitch   ,  mm);
  SETU(mesh_thick       ,  mm);
  SETU(mesh_hex_inradius,  mm);
  SETU(thin_wire_pitch  ,  mm);
  SETU(thin_wire_diam   ,  mm);
  SETU(thin_wire_rot    , deg);
  SETU(sipm_size        ,  mm);
  SETU(sipm_thick       ,  mm);
  SETU(sipm_gap         ,  mm);
  SET (n_sipm_side           );
  SETU(cath_thick       ,  mm);
  SETU(frame_thick_mesh ,  mm);
  SETU(frame_thick_wires,  mm);
  SETU(frame_width      ,  mm);
  SETU(neck_length      ,  mm);
  SET (fc_rings              );
  SETU(fc_ring_zpitch   ,  mm);
  SETU(fc_ring_width    ,  mm);
  SETU(fc_ring_thick    ,  mm);
  SET (sipms_on_fp           );
  SET ( ptfe_on_fp           );
  SET(ptfe_on_walls          );

  SETU(calib_belt_router    ,  mm);
  SETU(calib_belt_rinner    ,  mm);
  SETU(calib_belt_separation,  mm);
  SET (calib_belt_loops          );
#undef SETU
#undef SET

  set_seed(s_.seed);
}

void messenger::set_seed(u64 s) {
  s_.seed = s;
  G4Random::setTheSeed(s_.seed);
}

void messenger::set_start_id(u64 id) {
  s_.start_id = id;
  START_ID = id;
}

void messenger::set_calib_belt(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);

  if (str==    "NONE") { g_.calib_belt =  CalibrationBelt::NONE    ; return; }
  if (str=="STRAIGHT") { g_.calib_belt =  CalibrationBelt::STRAIGHT; return; }
  if (str==  "SPIRAL") { g_.calib_belt =  CalibrationBelt::SPIRAL  ; return; }

  G4Exception("[messenger::set_calib_belt]", "", FatalErrorInArgument, "Unknown value");
}
