#include "generators/scalar.hh"
#include "materials/LXe.hh"
#include "types.hh"

#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <G4SystemOfUnits.hh>

#include <n4-constants.hh>
#include <n4-random.hh>
#include <n4-sequences.hh>

#include <cstdlib>
#include <cassert>
#include <numeric>


lxe_scintillation::lxe_scintillation()
{
  auto n   = 101;
  auto wls = n4::linspace(165 * nm, 185 * nm, n);
  energy_  = n4::map<f64>([](f64 wl) {return c4::hc/wl;}, wls);

  auto pdf    = n4::map<f64>(LXe_Scintillation, energy_);
  auto pdfsum = std::accumulate(pdf.cbegin(), pdf.cend(), 0.);
  assert((pdfsum > 0) && "PDF does not have a strictly positive sum");

  cdf_.reserve(n);
  cdf_.push_back(pdf[0] / pdfsum);
  for (auto i=1; i<pdf.size(); i++)
    cdf_.push_back(cdf_[i-1] + pdf[i] / pdfsum);

  half_bin_size = std::abs(energy_[1] - energy_[0]) / 2;
}

f64 lxe_scintillation::generate() const {
  auto x = n4::random::uniform(0, 1);

  for (auto [i, cdf]: n4::enumerate(cdf_))
    if (x < cdf)
      return energy_[i] + n4::random::uniform(-half_bin_size, half_bin_size);

  /// unreachable!
  G4Exception("[lxe_scintillation::generate]", "", FatalException, "UNREACHABLE");
  return -1;
}
