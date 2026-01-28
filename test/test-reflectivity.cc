#include <n4-all.hh>

#include <G4OpticalPhysics.hh>
#include <G4StepLimiterPhysics.hh>
#include <G4SystemOfUnits.hh>
#include <G4Step.hh>

#include <G4OpticalSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4SurfaceProperty.hh>

#include "core/common.hh"
#include "core/utils.hh"
#include "generators/generic.hh"
#include "materials/LXe.hh"
#include "materials/silicon.hh"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>


using namespace Catch::Matchers;


TEST_CASE("reflectivity of silicon at normal incidence without surfaces", "[reflectivity][silicon][normal]") {
  auto lxe   = LXe_with_properties();
  auto si    = silicon_with_properties();
  auto nphot = 10000;

  auto geometry = [&] {
    auto liquid = n4::box("liquid").cube(10 * mm).place(lxe).now();
    auto sipm   = n4::box("sipm").cube(10 * mm).z(1 * mm).place(si).in(liquid).now();
    return liquid;
  };

  auto physics = [&] {
    auto phys_list = new FTFP_BERT{0};
    phys_list -> ReplacePhysics(new G4EmStandardPhysics_option4{0});
    phys_list -> ReplacePhysics(new G4OpticalPhysics{0});
    phys_list -> ReplacePhysics(new G4StepLimiterPhysics{});
    return phys_list;
  };

  auto nabsorbed  = 0;
  auto nreflected = 0;
  auto actions = [&] {
    auto gen = (new generic_generator("opticalphoton", nphot))
    -> fix_pos({0, 0, -5})
    -> fix_dir({0, 0,  1})
    -> pol(std::make_unique<n4::random::direction>())
    -> fix_ene(7.21 * eV)
    ;

    auto stepping = [&](const G4Step *step) {
      auto pre  = get_steppoint_volume( step->GetPreStepPoint() );
      auto post = get_steppoint_volume(step->GetPostStepPoint());
      auto pds = step->GetPostStepPoint()->GetProcessDefinedStep();
      auto proc = pds ? pds -> GetProcessName() : "No process";

      if ((pre == "sipm") && (post == "liquid"))
        nreflected++;

      if (proc == "OpAbsorption")
        nabsorbed++;

    };
    return (new n4::actions{gen})
      ->set(new n4::stepping_action{stepping})
      ;
  };

  n4::run_manager::create()
      .fake_ui()
      .physics(physics())
      .geometry(geometry)
      .actions(actions())
    .run(1);

  auto reflectivity_expected = std::pow((1.75 - 1.5) / (1.75 + 1.5), 2);
  auto reflectivity_got      = nreflected / static_cast<f64>(nphot);
  CHECK_THAT(reflectivity_got, WithinAbs(reflectivity_expected, 0.001));
  CHECK     (nreflected + nabsorbed  == nphot);
}


TEST_CASE("reflectivity at normal incidence with a layer", "[reflectivity][layer][normal]") {
  auto lxe   = LXe_with_properties();
  auto si    = silicon_with_properties();
  auto nphot = 10000;
  auto ref   = 0.25;

  auto geometry = [&] {
    auto liquid    = n4::box("liquid"   ).cube(10 * mm)            .place(lxe)              .now();
    auto interface = n4::box("interface").cube(10 * mm).z(1.001*mm).place(lxe).in(liquid   ).now();
    auto sipm      = n4::box("sipm"     ).cube(10 * mm).z(1 * mm)  .place(si ).in(interface).now();

    auto reflector = n4::material_properties{}
      .add("REFLECTIVITY"         , OPTPHOT_E_RANGE, { ref, ref})
      .add("TRANSMITTANCE"        , OPTPHOT_E_RANGE, {  1 ,  1 })
      .add("SPECULARLOBECONSTANT" , OPTPHOT_E_RANGE, {  0.,  0.})
      .add("SPECULARSPIKECONSTANT", OPTPHOT_E_RANGE, {  1.,  1.})
      .add("BACKSCATTERCONSTANT"  , OPTPHOT_E_RANGE, {  0.,  0.})
      .done();

    auto surface = new G4OpticalSurface("reflector", unified, polished, dielectric_metal);
    surface -> SetMaterialPropertiesTable(reflector);

    new G4LogicalBorderSurface("reflector", liquid, interface, surface);
    return liquid;
  };

  auto physics = [&] {
    auto phys_list = new FTFP_BERT{0};
    phys_list -> ReplacePhysics(new G4EmStandardPhysics_option4{0});
    phys_list -> ReplacePhysics(new G4OpticalPhysics{0});
    phys_list -> ReplacePhysics(new G4StepLimiterPhysics{});
    return phys_list;
  };

  auto nabsorbed  = 0;
  auto nreflected = 0;
  auto actions = [&] {
    auto gen = (new generic_generator("opticalphoton", nphot))
    -> fix_pos({0, 0, -5})
    -> fix_dir({0, 0,  1})
    -> pol(std::make_unique<n4::random::direction>())
    -> fix_ene(7.21 * eV)
    ;

    auto stepping = [&](const G4Step *step) {
      auto pre  = get_steppoint_volume( step->GetPreStepPoint() );
      auto post = get_steppoint_volume(step->GetPostStepPoint());
      auto pds  = step->GetPostStepPoint()->GetProcessDefinedStep();
      auto proc = pds ? pds -> GetProcessName() : "No process";

      if ((pre == "interface") && (post == "liquid"))
        nreflected++;

      if (proc == "OpAbsorption")
        nabsorbed++;

    };
    return (new n4::actions{gen})
      ->set(new n4::stepping_action{stepping})
      ;
  };

  n4::run_manager::create()
      .fake_ui()
      .physics(physics())
      .geometry(geometry)
      .actions(actions())
    .run(1);

  // intrinsic reflectivity due to ref index
  auto reflectivity_si       = std::pow((1.75 - 1.5) / (1.75 + 1.5), 2);
  // reflectivity defined by layer + transmittance * intrinsic reflectivity * transmitance
  auto reflectivity_expected = ref + (1-ref) * reflectivity_si * (1 - ref);

  auto reflectivity_got      = nreflected / static_cast<f64>(nphot);
  CHECK_THAT(reflectivity_got, WithinAbs(reflectivity_expected, 0.001));
  CHECK     (nreflected + nabsorbed  == nphot);
}
