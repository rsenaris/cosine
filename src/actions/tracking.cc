#include "actions/tracking.hh"
#include "types.hh"

#include <G4Track.hh>

#include <n4-inspect.hh>

#include <vector>

std::vector<Track> TRACKS;


std::function<void(const G4Track*)> store_tracks() {
  return [](const G4Track *track) {
    auto event = n4::event_number();
    auto track_id = track->GetTrackID();
    auto time = track -> GetGlobalTime();
    TRACKS.push_back(make_track(event, track_id, time));
  };
}
