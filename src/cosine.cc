#include <G4SystemOfUnits.hh>
#include <G4GenericMessenger.hh>
#include <G4SystemOfUnits.hh>   // physical units such as `m` for metre
#include <G4Event.hh>           // needed to inject primary particles into an event
#include <FTFP_BERT.hh>         // our choice of physics list
#include <G4GenericPhysicsList.hh>         // our choice of physics list
#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <Randomize.hh>

#include "actions/event.hh"
#include "actions/store_volume_crossing.hh"
#include "actions/tracking.hh"
#include "core/config.hh"
#include "core/messenger.hh"
#include "core/utils.hh"
#include "generators/selector.hh"
#include "geometry/pcolina.hh"
#include "persistency/manager.hh"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <n4-all.hh>
#include <n4-random.hh>

#include <cstdlib>
#include <memory>


n4::actions* create_actions(u32 nphot, const sim_config& s, const geometry_config& g) {

  auto begin_event = count_event();
  if (s.store_sources) begin_event = join(begin_event, store_primaries());

  auto gen     = select_generator(s, g);
  auto actions = (new n4::actions{gen.release()})
    ->set((new n4::event_action{})
          ->begin(begin_event)
          ->end(store_event(s)));

  if (s.store_tracks)
    actions = actions -> set((new n4::tracking_action{}) -> pre(store_tracks()));

  if (s.store_steps)
    actions = actions -> set(new n4::stepping_action{store_volume_crossing("geantino", "", "gate")});

  return actions;
}

int main(int argc, char* argv[]) {

  auto physics_list = std::make_unique<G4GenericPhysicsList>(0);

  auto geoconf = geometry_config::colina();
  auto simconf =      sim_config::empty();
  auto msg     = messenger(simconf, geoconf);

  PersistencyManager::Initialize(simconf.outputfile);

  using clock = std::chrono::high_resolution_clock;
  auto  t0    = clock::now();

  n4::run_manager::create()
    .ui("cosine", argc, argv)
    .macro_path("macs")
    .apply_early_macro("physics_em_optical.mac")
    .apply_cli_early() // CLI --early executed at this point
    // Important! physics list has to be set before the generator!
    .physics(physics_list.release())
    .geometry([&geoconf]() { return pcolina(geoconf); })
    .actions(create_actions(simconf.nparticles, simconf, geoconf))

    .apply_cli_late() // CLI --late executed at this point

    .run();

  auto writer = PERSISTENCY_MANAGER.get() -> writer();
  writer -> write_config(simconf.parameter_list());
  writer -> write_config(geoconf.parameter_list());


  auto t1 = clock::now();
  auto dt = std::chrono::duration<float>(t1 - t0).count();
  char message[1000];
  sprintf(message, "Program run in %d min, %f s", static_cast<u16>(std::floor(dt/60)), std::fmod(dt, 60));
  std::cout << message << std::endl;
}
