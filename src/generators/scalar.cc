#include "core/types.hh"
#include "generators/scalar.hh"
#include "materials/LAr.hh"
#include "materials/LXe.hh"

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
  auto [energy, pdf] = n4::interpolate(LXe_Scintillation, 200, c4::hc/(185*nm), c4::hc/(165*nm));
  energy_ = energy;

  auto pdfsum = std::accumulate(pdf.cbegin(), pdf.cend(), 0.);
  assert((pdfsum > 0) && "PDF does not have a strictly positive sum");

  cdf_.reserve(pdf.size());
  cdf_.push_back(pdf[0] / pdfsum);
  for (auto i=1u; i<pdf.size(); i++)
    cdf_.push_back(cdf_[i-1] + pdf[i] / pdfsum);

  bin_size = std::abs(energy_[1] - energy_[0]);
}

f64 lxe_scintillation::generate() const {
  auto x = n4::random::uniform(0, 1);

  for (auto [i, cdf] : n4::enumerate(cdf_))
    if (x < cdf)
      return energy_[i] + n4::random::uniform(0., bin_size);

  /// unreachable!
  G4Exception("[lxe_scintillation::generate]", "", FatalException, "UNREACHABLE");
  return -1;
}


lar_scintillation::lar_scintillation()
{
  auto [energy, pdf] = n4::interpolate(LAr_Scintillation, 200, c4::hc/(140*nm), c4::hc/(110*nm));
  energy_ = energy;

  auto pdfsum = std::accumulate(pdf.cbegin(), pdf.cend(), 0.);
  assert((pdfsum > 0) && "PDF does not have a strictly positive sum");

  cdf_.reserve(pdf.size());
  cdf_.push_back(pdf[0] / pdfsum);
  for (auto i=1u; i<pdf.size(); i++)
    cdf_.push_back(cdf_[i-1] + pdf[i] / pdfsum);

  bin_size = std::abs(energy_[1] - energy_[0]);
}

f64 lar_scintillation::generate() const {
  auto x = n4::random::uniform(0, 1);

  for (auto [i, cdf]: n4::enumerate(cdf_))
    if (x < cdf)
      return energy_[i] + n4::random::uniform(0., bin_size);

  /// unreachable!
  G4Exception("[lar_scintillation::generate]", "", FatalException, "UNREACHABLE");
  return -1;
}
