#pragma once

#include "G4VPersistencyManager.hh"
#include "persistency/hdf5_writer.hh"
#include <memory>

/// Important!!!
/// The input strings must be non-const references in order to allow them to
/// change from macro or CLI

class PersistencyManager : public G4VPersistencyManager {
private:
   PersistencyManager(std::string &filename) : G4VPersistencyManager(), writer_(filename) {}
public:
  ~PersistencyManager(){}

  static void Initialize(std::string& filename);

  G4bool Store(const G4Run*            ) override { return false; }
  G4bool Store(const G4Event*          ) override { return false; }
  G4bool Store(const G4VPhysicalVolume*) override { return false; }

  G4bool Retrieve(G4Run*&             ) override { return false; }
  G4bool Retrieve(G4Event*&           ) override { return false; }
  G4bool Retrieve(G4VPhysicalVolume *&) override { return false; }

  HDF5Writer* writer() { return &writer_; }

private:
  HDF5Writer writer_;
};

extern std::unique_ptr<PersistencyManager> PERSISTENCY_MANAGER;
