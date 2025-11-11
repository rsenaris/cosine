#pragma once

#include "generators/position.hh"
#include "generators/scalar.hh"
#include "types.hh"

#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
#include <G4VUserPrimaryGeneratorAction.hh>

#include <n4-random.hh>

#include <memory>
#include <variant>


#define random_direction n4::random::direction

struct generic_generator : G4VUserPrimaryGeneratorAction {
   generic_generator(const G4String &particle_name, u16 nparticles);
  ~generic_generator() = default;

  G4PrimaryVertex* generate_vertex() const;

  // move-only
  generic_generator(const generic_generator&) = delete;
  generic_generator& operator=(const generic_generator&) = delete;
  generic_generator(generic_generator&&) noexcept = default;
  generic_generator &operator=(generic_generator &&) noexcept = default;

  void GeneratePrimaries(G4Event *event) override { event -> AddPrimaryVertex(generate_vertex()); }

  generic_generator* fix_pos(const G4ThreeVector& vec) { pos_ = vec; return this;}
  generic_generator* fix_dir(const G4ThreeVector& vec) { dir_ = vec; return this;}
  generic_generator* fix_pol(const G4ThreeVector& vec) { pol_ = vec; return this;}
  generic_generator* fix_ene(G4double             val) { ene_ = val; return this;}

  generic_generator* pos(std::unique_ptr<random_position> && gen) { pos_ = std::move(gen); return this;}
  generic_generator* dir(std::unique_ptr<random_direction>&& gen) { dir_ = std::move(gen); return this;}
  generic_generator* pol(std::unique_ptr<random_direction>&& gen) { pol_ = std::move(gen); return this;}
  generic_generator* ene(std::unique_ptr<random_scalar>   && gen) { ene_ = std::move(gen); return this;}


private:
  #define GET(VAR, T1, T2) VAR.index() == 0 ? std::get<T1>(VAR) : std::get<std::unique_ptr<T2>>(VAR) -> get()

  G4ThreeVector generate_position    () const { return GET(pos_, G4ThreeVector, random_position ); }
  G4ThreeVector generate_direction   () const { return GET(dir_, G4ThreeVector, random_direction); }
  G4ThreeVector generate_polarization() const { return GET(pol_, G4ThreeVector, random_direction); }
  G4double      generate_energy      () const { return GET(ene_, G4double     , random_scalar   ); }

// G4ThreeVector generate_position    () const { return pos_.index() == 0 ? std::get<G4ThreeVector>(pos_) : std::get<vector_sampler>       (pos_).get(); }
// G4ThreeVector generate_direction   () const { return dir_.index() == 0 ? std::get<G4ThreeVector>(dir_) : std::get<n4::random::direction>(dir_).get(); }
// G4ThreeVector generate_polarization() const { return pol_.index() == 0 ? std::get<G4ThreeVector>(pol_) : std::get<n4::random::direction>(pol_).get(); }
// G4double      generate_energy      () const { return ene_.index() == 0 ? std::get<G4double>     (ene_) : std::get<scalar_sampler>       (ene_).get(); }

  #undef GET

  G4ParticleDefinition* particle_;
  u16 nparticles_;
  std::variant<G4ThreeVector, std::unique_ptr<random_position >> pos_;
  std::variant<G4ThreeVector, std::unique_ptr<random_direction>> dir_;
  std::variant<G4ThreeVector, std::unique_ptr<random_direction>> pol_;
  std::variant<G4double     , std::unique_ptr<random_scalar   >> ene_;
};

#undef random_direction
