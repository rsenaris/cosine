#pragma once

#include "types.hh"

#include <G4Track.hh>

#include <functional>

std::function<void(const G4Track*)> store_tracks();

extern std::vector<Track> TRACKS;
