#include "core/config.hh"
#include "core/symbols.hh"
#include "generators/generic.hh"
#include "generators/selector.hh"

#include <n4-random.hh>

#include <G4VUserPrimaryGeneratorAction.hh>

#include <memory>


std::unique_ptr<G4VUserPrimaryGeneratorAction> select_generator(const sim_config& s, const geometry_config& g) {
  auto isotropic = std::make_unique<n4::random::direction>();
  auto randompol = std::make_unique<n4::random::direction>();

  G4VUserPrimaryGeneratorAction* gen{nullptr};

  switch (s.generator) {

  case EventGenerator::S1: {
    auto pos = std::make_unique<conical_volume_generator>(g.drift_length, g.el_r(), g.cath_r());
    pos -> offset_z(g.neck_length + g.drift_length / 2);

    gen = (new generic_generator("opticalphoton", s.nparticles))
      -> pos(std::move(pos))
      -> dir(std::make_unique<n4::random::direction>())
      -> pol(std::make_unique<n4::random::direction>())
      //    -> ene(std::make_unique<lxe_scintillation>())
      -> fix_ene(7.21 * eV)
    ;
    break;
  }
  case EventGenerator::S2: {
    auto pos = std::make_unique<el_generator>(g, 40 * um);
    pos -> offset_z(-g.mesh_thick -g.d_gate_wire);

    gen = (new generic_generator("opticalphoton", s.nparticles))
      -> pos(std::move(pos))
      -> dir(std::make_unique<n4::random::direction>())
      -> pol(std::make_unique<n4::random::direction>())
      //    -> ene(std::make_unique<lxe_scintillation>())
      -> fix_ene(7.21 * eV)
    ;
    break;
  }

  case EventGenerator::KR: {
    auto pos = std::make_unique<conical_volume_generator>(g.drift_length, g.el_r(), g.cath_r());
    pos -> offset_z(g.neck_length + g.drift_length / 2);

    gen = (new generic_generator("e-", 1))
      -> pos(std::move(pos))
      -> dir(std::make_unique<n4::random::direction>())
      -> pol(std::make_unique<n4::random::direction>())
      -> fix_ene(41.5585 * keV)
    ;
    break;
  }
  }

  // Repeating the return type for the compiler, who was otherwise unsure
  return std::unique_ptr<G4VUserPrimaryGeneratorAction>{gen};
}
