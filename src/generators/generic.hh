#pragma once

#include "types.hh"

#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>

#include <n4-random.hh>

#include <memory>
#include <variant>
#include <vector>


typedef std::vector<G4ThreeVector(void)> VectorGen;
typedef std::vector<G4double     (void)> ScalarGen;


struct vector_sampler {
  virtual G4ThreeVector get() const = 0;
  virtual ~vector_sampler() = default;
};

struct scalar_sampler {
  virtual G4double get() const = 0;
  virtual ~scalar_sampler() = default;
};


typedef std::unique_ptr<vector_sampler> vector_sampler_ptr;
typedef std::unique_ptr<scalar_sampler> scalar_sampler_ptr;

struct GenericGenerator {
  GenericGenerator(const G4String &particle_name, u16 nparticles);

  GenericGenerator& fix_pos(const G4ThreeVector& vec) { pos_ = vec; return *this;}
  GenericGenerator& fix_dir(const G4ThreeVector& vec) { dir_ = vec; return *this;}
  GenericGenerator& fix_pol(const G4ThreeVector& vec) { pol_ = vec; return *this;}
  GenericGenerator& fix_ene(G4double             val) { ene_ = val; return *this;}

  GenericGenerator& pos(vector_sampler_ptr      gen) { pos_ = std::move(gen); return *this;}
  GenericGenerator& dir(n4::random::direction&& gen) { dir_ = std::move(gen); return *this;}
  GenericGenerator& pol(n4::random::direction&& gen) { pol_ = std::move(gen); return *this;}
  GenericGenerator& ene(scalar_sampler_ptr      gen) { ene_ = std::move(gen); return *this;}

private:
  #define GETREF(VAR, T1, T2) VAR.index() == 0 ? std::get<T1>(VAR) : std::get<T2>(VAR) .  get()
  #define GETPTR(VAR, T1, T2) VAR.index() == 0 ? std::get<T1>(VAR) : std::get<T2>(VAR) -> get()

  G4ThreeVector generate_position    () const { return GETPTR(pos_, G4ThreeVector, vector_sampler_ptr   ); }
  G4ThreeVector generate_direction   () const { return GETREF(dir_, G4ThreeVector, n4::random::direction); }
  G4ThreeVector generate_polarization() const { return GETREF(pol_, G4ThreeVector, n4::random::direction); }
  G4double      generate_energy      () const { return GETPTR(ene_, G4double     , scalar_sampler_ptr   ); }

// G4ThreeVector generate_position    () const { return pos_.index() == 0 ? std::get<G4ThreeVector>(pos_) : std::get<vector_sampler>       (pos_).get(); }
// G4ThreeVector generate_direction   () const { return dir_.index() == 0 ? std::get<G4ThreeVector>(dir_) : std::get<n4::random::direction>(dir_).get(); }
// G4ThreeVector generate_polarization() const { return pol_.index() == 0 ? std::get<G4ThreeVector>(pol_) : std::get<n4::random::direction>(pol_).get(); }
// G4double      generate_energy      () const { return ene_.index() == 0 ? std::get<G4double>     (ene_) : std::get<scalar_sampler>       (ene_).get(); }

  #undef GETPTR
  #undef GETREF

  G4ParticleDefinition* particle_;
  u16 nparticles_;
  std::variant<G4ThreeVector, vector_sampler_ptr>    pos_;
  std::variant<G4ThreeVector, n4::random::direction> dir_;
  std::variant<G4ThreeVector, n4::random::direction> pol_;
  std::variant<G4double     , scalar_sampler_ptr>    ene_;
};
