#pragma once

#include <G4ThreeVector.hh>

#include <memory>

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
