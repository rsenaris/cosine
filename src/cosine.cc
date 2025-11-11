#include <G4SystemOfUnits.hh>
#include <G4GenericMessenger.hh>
#include <G4SystemOfUnits.hh>   // physical units such as `m` for metre
#include <G4Event.hh>           // needed to inject primary particles into an event
#include <FTFP_BERT.hh>         // our choice of physics list

#include "actions/event.hh"
#include "actions/store_volume_crossing.hh"
#include "generators/generic.hh"
#include "generators/position.hh"
#include "geometry/pcolina.hh"
#include "persistency/manager.hh"

#include <n4-all.hh>
#include <n4-random.hh>

#include <cstdlib>
#include <memory>

n4::actions *create_actions(u16 nphot) {
  auto pos = std::make_unique<conical_volume_generator>(12 * cm, 32 * mm, 152 * mm);
  pos -> offset_z(6*cm);

  auto gen = new generic_generator("opticalphoton", nphot);

  return  (  new n4::        actions{ gen })
    -> set( (new n4::   event_action{}) -> begin(count_event()) -> end(store_event()))
    -> set(  new n4::stepping_action{store_volume_crossing("geantino", "", "gate")});
}

int main(int argc, char* argv[]) {
  G4int physics_verbosity = 0;

  // The trailing slash after '/my_geometry' is CRUCIAL: without it, the
  // messenger violates the principle of least surprise.
  auto messenger = new G4GenericMessenger{nullptr, "/my/", "docs: bla bla bla"};
  messenger -> DeclareProperty        ("physics_verbosity" ,        physics_verbosity );

  PersistencyManager::Initialize("output_file.h5", 0);

  n4::run_manager::create()
    .ui("cosine", argc, argv)
    .macro_path("macs")
    .apply_cli_early() // CLI --early executed at this point

    // Important! physics list has to be set before the generator!
    .physics<FTFP_BERT>(physics_verbosity)
    .geometry(pcolina)
    .actions(create_actions(1))

    .apply_cli_late() // CLI --late executed at this point

    .run();


}
