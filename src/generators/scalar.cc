#include "generators/scalar.hh"
#include "materials/LXe.hh"

#include <G4SystemOfUnits.hh>
#include <n4-random.hh>
#include <n4-sequences.hh>
#include <numeric>


lxe_scintillation::lxe_scintillation()
{
  auto n = 100;
  energy_ = n4::linspace(165 * nm, 185 * nm, n);

  auto pdf = n4::map<f64>(LXe_Scintillation, energy_);
  cdf_.reserve(n);

  auto pdfsum = std::accumulate(pdf.cbegin(), pdf.cend(), 0);
  cdf_[0] = pdf[0] / pdfsum;
  for (auto i=1; i<pdf.size(); i++) {
    cdf_[i] = cdf_[i-1] + pdf[i] / pdfsum;
  }
  half_bin_size = (energy_[1] - energy_[0]) / 2;
}

f64 lxe_scintillation::generate() const {
  auto x = n4::random::uniform(0, 1);

  for (auto [i, cdf]: n4::enumerate(cdf_))
    if (x < cdf)
      return energy_[i] + n4::random::uniform(-half_bin_size, half_bin_size);

  /// unreachable!
  return -1;
}
