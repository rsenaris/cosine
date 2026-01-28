#pragma once

#include "core/config.hh"

#include <G4VUserPrimaryGeneratorAction.hh>

#include <cstring>
#include <memory>

std::unique_ptr<G4VUserPrimaryGeneratorAction> select_generator(const sim_config& s, const geometry_config& g);
