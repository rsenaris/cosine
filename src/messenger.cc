#include "config.hh"
#include "messenger.hh"

#include "G4GenericMessenger.hh"
#include "Randomize.hh"

#include <memory>

messenger::messenger(sim_config &s, geometry_config &g)
    : s_(s), g_(g), msg_(nullptr)
{
  msg_ = std::make_unique<G4GenericMessenger>(this, "/sim/", "Simulation parameters");

  msg_->DeclareMethod("seed", &messenger::set_seed, "Set the random seed");

  set_seed(s_.seed);
}

void messenger::set_seed(u64 s) {
  s_.seed = s;
  G4Random::setTheSeed(s_.seed);
}
