#pragma once

#include "types.hh"

#include <G4ThreeVector.hh>

#include <n4-random.hh>

#include <cmath>
#include <memory>


struct random_position {
  virtual G4ThreeVector generate() const = 0;
  virtual ~random_position() = default;

  G4ThreeVector get() const { return generate() + offset_; }

  random_position& offset  (f64 x, f64 y, f64 z){ offset_ = {        x  ,         y  ,         z  }; return (*this); }
  random_position& offset_x(f64 x              ){ offset_ = {        x  , offset_.y(), offset_.z()}; return (*this); }
  random_position& offset_y(       f64 y       ){ offset_ = {offset_.x(),         y  , offset_.z()}; return (*this); }
  random_position& offset_z(              f64 z){ offset_ = {offset_.x(), offset_.y(),         z  }; return (*this); }

private:
  G4ThreeVector offset_;
};

struct union_random_position : public random_position {
  typedef std::vector<std::unique_ptr<random_position>> GenVec;

   union_random_position(GenVec&& gens, std::vector<f32> weights);
   union_random_position(std::unique_ptr<random_position>&&, std::unique_ptr<random_position>&&, f32 w1, f32 w2);
  ~union_random_position() override {};

  G4ThreeVector generate() const override;

private:
  GenVec           gens;
  std::vector<f32> weights;
};

struct conical_volume_generator : public random_position {
  conical_volume_generator(f64 length, f64 r1, f64 r2)
      : hlength_(length/2), r1_(r1), r2_(r2), r1_2(r1*r1), r2_2(r2*r2) {}

  ~conical_volume_generator() override {}
  G4ThreeVector generate() const override;

private:
  f64 hlength_;
  f64 r1_, r2_, r1_2, r2_2;
};

struct cylindrical_volume_generator : public random_position {
  cylindrical_volume_generator(f64 length, f64 r1, f64 r2)
      : hlength_(length/2), r1_2(r1*r1), r2_2(r2*r2) {}

  ~cylindrical_volume_generator() override {}
  G4ThreeVector generate() const override;

private:
  f64 hlength_;
  f64 r1_2, r2_2;

  f64 r2_at(f64 z) {
    return 0;
  }
};
