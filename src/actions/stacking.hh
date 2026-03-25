#pragma once

#include <G4ClassificationOfNewTrack.hh>

#include <G4Track.hh>

#include <functional>

std::function<G4ClassificationOfNewTrack(G4Track const*)> kill_photons();
