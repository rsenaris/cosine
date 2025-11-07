#pragma once

#include "G4ThreeVector.hh"
#include "generators/generic.hh"
#include "types.hh"
#include <n4-random.hh>
#include <utility>


struct conical_volume_generator : public vector_sampler {
  conical_volume_generator(f64 z1, f64 z2, f64 r1, f64 r2)
      : z1_(z1), z2_(z2), r1_(r1), r2_(r2), flip_(r2 < r1) {
    if (flip_)
      std::swap(r1_, r2_);
  }

  ~conical_volume_generator() override {}

  G4ThreeVector get() const override;

private:
  f64 z1_, z2_;
  f64 r1_, r2_;
  bool flip_;

  f64 r2_at(f64 z) {
    return 0;
  }
}


G4ThreeVector conical_volume_generator::get() const {
  auto [x, y] = n4::random::random_on_disc(r1_+ r2_);
  auto z = n4::random::uniform(z1_, z2_);

  auto r2 = x * x + y * y;
  if (r2 < r1_*r1_) return {x, y, z};

  if (r2_at(z) < r2) {
    z = z2_ - z;
    r = r1_ + r2_ - r;

  }
}
