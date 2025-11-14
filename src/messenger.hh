#pragma once

#include "config.hh"
#include "types.hh"

#include <G4GenericMessenger.hh>

#include <memory>

struct messenger {
   messenger(sim_config &s, geometry_config &g);
  ~messenger() {}

  void set_seed    (u64);
  void set_start_id(u64);

private:
  sim_config& s_;
  geometry_config& g_;
  std::unique_ptr<G4GenericMessenger> msg_;
};
