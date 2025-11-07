#pragma once

#include "types.hh"

#include <n4-sensitive.hh>

#include <memory>
#include <vector>

std::unique_ptr<n4::sensitive_detector> sensitive_sipm();

extern std::vector<SensorHit> sensor_hits;
