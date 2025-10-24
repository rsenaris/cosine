#pragma once

#include <G4Types.hh>
#include <G4ThreeVector.hh>

#include <cstdint>

// Rust naming, much more concise
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8  = std:: int8_t;
using i16 = std:: int16_t;
using i32 = std:: int32_t;
using i64 = std:: int64_t;
using f32 =  float;
using f64 = double;


static const unsigned SHORTSTR =  30;
static const unsigned  LONGSTR = 300;

using shortstr = char[SHORTSTR];
using  longstr = char[ LONGSTR];

struct StepData {
  shortstr particle;
  shortstr  pre_volume;
  shortstr post_volume;
  f32       pre_x;
  f32       pre_y;
  f32       pre_z;
  f32      post_x;
  f32      post_y;
  f32      post_z;

  StepData( const G4String&
          , const G4String&
          , const G4String&
          , const G4ThreeVector&
          , const G4ThreeVector&);
};
