#include "G4ThreeVector.hh"
#include "core/config.hh"
#include "core/symbols.hh"
#include "generators/generic.hh"
#include "generators/position.hh"
#include "generators/scalar.hh"
#include "generators/selector.hh"

#include <n4-random.hh>

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4IonTable.hh>

#include <memory>


auto scint_energy(Medium m) {
  switch (m) {
    case Medium::XENON     : return 7.21 * eV;
    case Medium::KRYPTON   : return 8.27 * eV;
    case Medium::ARGON     : return 9.69 * eV;
    case Medium::ARGONXENON: return 7.21 * eV;
  }
}

std::unique_ptr<random_scalar> scint_gen(Medium m) {
  switch (m) {
    case Medium::XENON     : return std::make_unique<lxe_scintillation>();
//    case Medium::KRYPTON   : return ;
    case Medium::ARGON     : return std::make_unique<lar_scintillation>();
    case Medium::ARGONXENON: return std::make_unique<lxe_scintillation>();
    default: break;
    }
  return nullptr;
}


std::unique_ptr<G4VUserPrimaryGeneratorAction> select_generator(const sim_config& s, const geometry_config& g) {
  auto isotropic = std::make_unique<n4::random::direction>();
  auto randompol = std::make_unique<n4::random::direction>();
  //auto sc_energy = scint_energy(g.medium);
  auto ene_generator = scint_gen(g.medium);
  G4VUserPrimaryGeneratorAction* gen{nullptr};

  switch (s.generator) {

  case EventGenerator::DEBUG: {
    auto pos = std::make_unique<fixed_position_generator>(G4ThreeVector{0., 0., 1.});

    gen = (new generic_generator("geantino", s.nparticles))
      -> pos(std::move(pos))
      -> dir(std::make_unique<n4::random::direction>())
      -> pol(std::make_unique<n4::random::direction>())
      //    -> ene(std::make_unique<lxe_scintillation>())
      -> fix_ene(1 * eV)
    ;
    break;
  }

  case EventGenerator::S1: {
    auto pos = std::make_unique<conical_volume_generator>(g.drift_length, g.el_r(), g.cath_r());
    pos -> offset_z(g.neck_length + g.drift_length / 2);

    gen = (new generic_generator("opticalphoton", s.nparticles))
      -> pos(std::move(pos))
      -> dir(std::make_unique<n4::random::direction>())
      -> pol(std::make_unique<n4::random::direction>())
      -> ene(std::move(ene_generator))
  //    -> fix_ene(sc_energy)
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
      -> ene(std::move(ene_generator))
      //-> fix_ene( sc_energy)
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

  case EventGenerator::FE: {
    auto gen_ = (new generic_generator(26, 55, 0.0, 1))
        -> dir(std::make_unique<n4::random::direction>())
        -> pol(std::make_unique<n4::random::direction>())
        -> fix_ene(0 * eV)
        ;
    if (s.vertex.has_value()) {
      auto pos = std::make_unique<fixed_position_generator>(s.vertex.value());
      pos -> offset_z(g.neck_length);
      gen = gen_ -> pos(std::move(pos));
    }
    else {
      auto offset = 0.1 * mm;
      auto pos = std::make_unique<cylindrical_volume_generator>(offset, 0.0, g.cath_r());
      pos -> offset_z(g.neck_length + g.drift_length - offset);

      gen = gen_ -> pos(std::move(pos));
    }
    break;
  }
  }

  // Repeating the return type for the compiler, who was otherwise unsure
  return std::unique_ptr<G4VUserPrimaryGeneratorAction>{gen};
}
