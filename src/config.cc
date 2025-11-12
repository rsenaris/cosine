#include "config.hh"

#include <algorithm>

const std::vector<f64>& geometry_config::wire_poss() const {
  static auto pos = std::vector<f64>{};
  if (pos.empty()) {
    auto half_n_wires = std::floor(el_diam/ 2 / thin_wire_pitch);

    for (auto i=0; i<half_n_wires; i++)
      for (auto s : {-1, 1})
        pos.push_back(s * thin_wire_pitch * (i + 0.5));

    std::sort(pos.begin(), pos.end());
  }
  return pos;
}

const std::vector<f64>& geometry_config::wire_lengths() const {
  static auto lens = std::vector<f64>{};
  if (lens.empty()) {
    for (auto& p: wire_poss())
      lens.push_back(2 * std::sqrt(el_r*el_r - p*p));
  }
  return lens;
}
