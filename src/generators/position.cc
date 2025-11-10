#include "generators/position.hh"

#include <G4ThreeVector.hh>

#include <n4-random.hh>

#include <memory>
#include <vector>


union_random_position::union_random_position(GenVec&& gens, std::vector<f32> weights)
    : gens   (std::move(gens))
    , weights(std::move(weights))
{
    auto wsum = std::accumulate(weights.cbegin(), weights.cend(), 0);
    for (auto i=0; i<weights.size(); i++) {
               weights[i] /= wsum;
      if (i>0) weights[i] += weights[i-1];
    }
}

// Oh boy, does C++ suck or what? I just want to create a vector from two
// pointers... but the initializer list does not work because it copies the
// items... So it's either this or creating a (non-anonymous) helper function to
// do it. I say: fuck C++, long live Rust!
union_random_position::union_random_position(std::unique_ptr<random_position>&& g1, std::unique_ptr<random_position>&& g2, f32 w1, f32 w2)
    : union_random_position([](auto &&a, auto &&b) {
      GenVec v;
      v.push_back(std::move(a));
      v.push_back(std::move(b));
      return v;
    }(std::move(g1), std::move(g2)),

      {w1, w2})
{}

G4ThreeVector union_random_position::generate() const {
  auto choice = n4::random::uniform(0., 1.);
  for (auto i=0; i<weights.size(); i++)
    if (weights[i] > choice)
      return gens[i] -> get();

  // unreachable!
  return {};
}

G4ThreeVector conical_volume_generator::generate() const {
  auto r = std::sqrt(n4::random::uniform(r1_2, r2_2));
  auto z = n4::random::uniform(-hlength_, hlength_);
  if (r > r1_) {
    auto slope_cone = (r2_ - r1_) / 2 / hlength_;
    auto slope_pos  = (r   - r1_) / (z + hlength_);
    if (slope_pos > slope_cone) {
      z = -z;
      r = r2_ - (r - r1_);
    }
  }
  auto p = n4::random::uniform(0, CLHEP::twopi);
  auto x = r * std::cos(p);
  auto y = r * std::sin(p);
  return {x, y, z};
}

G4ThreeVector cylindrical_volume_generator::generate() const {
  auto r = std::sqrt(n4::random::uniform(r1_2, r2_2));
  auto p = n4::random::uniform(0, CLHEP::twopi);
  auto z = n4::random::uniform(-hlength_, hlength_);
  auto x = r * std::cos(p);
  auto y = r * std::sin(p);
  return {x, y, z};
}
