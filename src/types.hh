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


////////////////////////////////////////////////////////////////////////////////
// Custom types
// Remember, these are transient representations. Persistent representations
// are stored in persistency/hdf5_types. If you need to store any of these types:
// - generate a struct here
// - it is usually useful to make a function-ctor here as well (.cc)
// - generate a HighFive::CompoundType in persistency/hdf5_types
// - generate a function-ctor for the HighFive::CompountType (in .cc)
////////////////////////////////////////////////////////////////////////////////


struct ConfPar {
  shortstr name;
   longstr value;
  shortstr unit;
};


struct VolumeChange {
  u64      event;
  shortstr particle;
  shortstr  pre_volume;
  shortstr post_volume;
  f32       pre_x;
  f32       pre_y;
  f32       pre_z;
  f32      post_x;
  f32      post_y;
  f32      post_z;
};

struct SensorHit {
  u64 event;
  u16 sensor_id;
  f32 time;
};

struct Interaction {
  u64 event;
  f32 x, y, z, e;
  u32 n;
};

VolumeChange make_volume_change(       u64
                               , const G4String&
                               , const G4String&
                               , const G4String&
                               , const G4ThreeVector&
                               , const G4ThreeVector&);

SensorHit make_sensor_hit( u64
                         , u16
                         , f32);

Interaction make_interaction( u64
                            , const G4ThreeVector &
                            , f32
                            , u32);

ConfPar make_confpar( const std::string &
                    , const std::string &
                    , const std::string &
                    );
