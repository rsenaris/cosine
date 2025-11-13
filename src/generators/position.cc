#include "config.hh"
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
  static auto gen_z = n4::random::piecewise_linear_distribution({0, 2*hlength_}, {r1_*r1_, r2_*r2_});
  static auto slope = (r2_ - r1_) / 2 / hlength_;

  auto z = gen_z.sample();
  auto u = r1_ + slope * z;
  auto r = std::sqrt(n4::random::uniform(0, u*u));
  auto p = n4::random::uniform(0, CLHEP::twopi);
  auto x = r * std::cos(p);
  auto y = r * std::sin(p);
  return {x, y, z - hlength_};
}

G4ThreeVector cylindrical_volume_generator::generate() const {
  auto r = std::sqrt(n4::random::uniform(r1_2, r2_2));
  auto p = n4::random::uniform(0, CLHEP::twopi);
  auto z = n4::random::uniform(-hlength_, hlength_);
  auto x = r * std::cos(p);
  auto y = r * std::sin(p);
  return {x, y, z};
}

el_generator::el_generator(const geometry_config& g, f64 range)
    : gen_(nullptr)
{
    std::vector<std::unique_ptr<random_position>> gens;
    std::vector<f32> probs;

    const auto& pos = g.wire_poss();
    const auto& length = g.wire_lengths();
    auto wire_r = g.thin_wire_diam / 2;

    auto norm = std::accumulate(length.cbegin(), length.cend(), 0);
    for (auto i = 0; i < pos.size(); i++) {
      auto gen = std::make_unique<cylindrical_volume_generator>(length[i], wire_r, wire_r + range);
      gen -> offset_x(pos[i]);

      gens .push_back(std::move(gen));
      probs.push_back(length[i] / norm);
    }

    gen_ = std::make_unique<union_random_position>(std::move(gens), probs);
    wire_rotation_ = g.thin_wire_rot;
}

G4ThreeVector el_generator::generate() const {
  // wires are laid in the yz plane, need to orient them correctly
  auto p = gen_->generate();
  p.rotateZ(CLHEP::halfpi);
  p.rotateY(wire_rotation_);
  return p;
}
