#pragma once

#include "G4VPersistencyManager.hh"
#include "persistency/hdf5_writer.hh"

class PersistencyManager : public G4VPersistencyManager {
private:
   PersistencyManager(const std::string &filename, G4int start_id) : G4VPersistencyManager(), writer_(filename, start_id) {}
public:
  ~PersistencyManager(){}

  static void Initialize(const std::string& filename, G4int start_id);

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
