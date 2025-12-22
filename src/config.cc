#include "config.hh"
#include "types.hh"

#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>

#include <algorithm>
#include <string>
#include <vector>

u64 START_ID = 0;

const std::vector<f64>& geometry_config::wire_poss() const {
  static auto pos = std::vector<f64>{};
  if (pos.empty()) {
    auto half_n_wires = std::floor(el_diam/ 2 / thin_wire_pitch);

    for (auto i=0u; i<half_n_wires; i++)
      for (auto s : {-1., 1.})
        pos.push_back(s * thin_wire_pitch * (i + 0.5));

    std::sort(pos.begin(), pos.end());
  }
  return pos;
}

const std::vector<f64>& geometry_config::wire_lengths() const {
  static auto lens = std::vector<f64>{};
  if (lens.empty()) {
    for (auto& p: wire_poss())
      lens.push_back(2 * std::sqrt(el_r()*el_r() - p*p));
  }
  return lens;
}

void geometry_config::check() const {
  if (cath_thick <= 2 * sipm_thick)
    G4Exception("[geometry_config::check]", "", FatalException
                , "Invalid geometry: cath_thick must be > 2*sipm_thick");

  if (mesh_hex_pitch - mesh_hex_inradius < 1*um)
    G4Exception("[geometry_config::check]", "", FatalException
                , "Invalid geometry: mesh_hex_pitch must be > mesh_hex_inradius");
}


template<class T>
std::string to_string(T val) { return std::to_string(val); }
std::string to_string(std::string val) { return val; }
std::string to_string(bool val) { return val ? "true" : "false"; }

#define UNITLESS(VAR      ) make_confpar(#VAR, to_string(VAR     ),    "")
#define WITHUNIT(VAR, UNIT) make_confpar(#VAR, to_string(VAR/UNIT), #UNIT)

std::vector<ConfPar> sim_config::parameter_list() {
  return {
    UNITLESS(outputfile),
    UNITLESS(generator),
    UNITLESS(nparticles),
    UNITLESS(seed),
    UNITLESS(start_id),
    UNITLESS(store_steps),
    UNITLESS(store_sens),
    UNITLESS(store_sources),
  };
}

std::vector<ConfPar> geometry_config::parameter_list() {
  return  {
    WITHUNIT(el_diam, mm),
    WITHUNIT(drift_length, mm),
    UNITLESS(form_factor),
    WITHUNIT(wall_thick, mm),
    WITHUNIT(d_gate_wire, mm),
    WITHUNIT(d_wire_shield, mm),
    WITHUNIT(d_shield_sipms, mm),
    WITHUNIT(mesh_hex_pitch, mm),
    WITHUNIT(mesh_thick, mm),
    WITHUNIT(mesh_hex_inradius, mm),
    WITHUNIT(thin_wire_pitch, mm),
    WITHUNIT(thin_wire_diam, mm),
    WITHUNIT(thin_wire_rot, deg),
    WITHUNIT(sipm_size, mm),
    WITHUNIT(sipm_thick, mm),
    WITHUNIT(sipm_gap, mm),
    UNITLESS(n_sipm_side),
    WITHUNIT(cath_thick, mm),
    WITHUNIT(frame_thick_mesh, mm),
    WITHUNIT(frame_thick_wires, mm),
    WITHUNIT(frame_width, mm),
    WITHUNIT(neck_length, mm),
    UNITLESS(fc_rings),
    WITHUNIT(fc_ring_zpitch, mm),
    WITHUNIT(fc_ring_width, mm),
    WITHUNIT(fc_ring_thick, mm),

    UNITLESS(  sipms_on_fp),
    UNITLESS(   ptfe_on_fp),
    UNITLESS(ptfe_on_walls),
  };
}

#undef UNITLESS
#undef WITHUNIT
