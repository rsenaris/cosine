#pragma once

#include "config.hh"

#include <n4-shape.hh>
#include <n4-vis-attributes.hh>

#include <G4PVPlacement.hh>

#include <cmath>

G4PVPlacement* straight_calibration_belt(const geometry_config& g, G4PVPlacement* liquid, n4::vis_attributes& attrs);
G4PVPlacement*   spiral_calibration_belt(const geometry_config& g, G4PVPlacement* liquid, n4::vis_attributes& attrs);
