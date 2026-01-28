#pragma once

#include "core/types.hh"

#include <vector>

struct random_scalar {
  virtual f64 generate() const = 0;
  virtual ~random_scalar() = default;

  f64 get() const { return generate() * scale_ + offset_; }

  template<class DERIVED> DERIVED& offset(f64 o){ offset_ = o; return (*this); }
  template<class DERIVED> DERIVED& scale (f64 s){ scale_  = s; return (*this); }

private:
  f64 scale_;
  f64 offset_;
};

struct lxe_scintillation : public random_scalar {
   lxe_scintillation();
  ~lxe_scintillation() override {}

  f64 generate() const override;


private:
  std::vector<f64> energy_;
  std::vector<f64> cdf_;
  f64 half_bin_size;
};
