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
#include "config.hh"
#include "generators/generic.hh"
#include "generators/position.hh"
#include "generators/scalar.hh"
#include "geometry/pcolina.hh"
#include "messenger.hh"
#include "persistency/manager.hh"

#include <n4-all.hh>
#include <n4-random.hh>

#include <cstdlib>
#include <memory>

#include "utils.hh"

n4::actions* create_actions(u32 nphot, const sim_config& s, const geometry_config& g) {

  std::unique_ptr<random_position> pos;

  if (s.generator == "s1") {
    pos = std::make_unique<conical_volume_generator>(g.drift_length, g.el_diam, g.cath_diam());
    pos -> offset_z(g.neck_length + g.drift_length/2);
  }
  else if (s.generator == "s2") {
    pos = std::make_unique<el_generator>(g, 40 * um);
    pos -> offset_z(-g.mesh_thick -g.d_gate_wire);
  }
  else {
    G4Exception("[create_actions]", "", FatalErrorInArgument, "Unknown generator");
  }

  auto gen = (new generic_generator("opticalphoton", nphot))
    -> pos(std::move(pos))
    -> dir(std::make_unique<n4::random::direction>())
    -> pol(std::make_unique<n4::random::direction>())
//    -> ene(std::make_unique<lxe_scintillation>())
    -> fix_ene(7.21 * eV)
    ;

  auto begin_event = count_event();
  if (s.store_interactions) begin_event = join(begin_event, store_primaries());

  auto actions = (new n4::actions{gen})
    ->set((new n4::event_action{})
          ->begin(begin_event)
          ->end(store_event(s)));

  if (s.store_steps)
    actions = actions->set(new n4::stepping_action{store_volume_crossing("geantino", "", "gate")});
  return actions;
}

int main(int argc, char* argv[]) {

  auto physics_list = std::make_unique<G4GenericPhysicsList>(0);

  auto geoconf = geometry_config::colina();
  auto simconf =      sim_config::s2();
  auto msg     = messenger(simconf, geoconf);

  PersistencyManager::Initialize(simconf.outputfile, 0);

  n4::run_manager::create()
    .ui("cosine", argc, argv)
    .macro_path("macs")
    .apply_cli_early() // CLI --early executed at this point

    // Important! physics list has to be set before the generator!
    .physics(physics_list.release())
    .geometry([&geoconf]() { return pcolina(geoconf); })
    .actions(create_actions(simconf.nparticles, simconf, geoconf))

    .apply_cli_late() // CLI --late executed at this point

    .run();
}
