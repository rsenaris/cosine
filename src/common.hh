#pragma once

#include<vector>

#include <G4Types.hh>
#include <G4SystemOfUnits.hh>

const G4double OPTPHOT_MIN_E_EV = 0.10;
const G4double OPTPHOT_MAX_E_EV = 8.21;
const G4double OPTPHOT_MIN_E    = OPTPHOT_MIN_E_EV * eV;
const G4double OPTPHOT_MAX_E    = OPTPHOT_MAX_E_EV * eV;
const G4double NO_ABSORPTION    = 1e8              *  m; // approx. infinity

const std::vector<G4double> OPTPHOT_E_RANGE_EV{OPTPHOT_MIN_E_EV, OPTPHOT_MAX_E_EV};
const std::vector<G4double> OPTPHOT_E_RANGE   {OPTPHOT_MIN_E, OPTPHOT_MAX_E};
