#include "persistency/manager.hh"
#include "persistency/hdf5_writer.hh"

void PersistencyManager::Initialize(const std::string &filename, G4int start_id) {
  if (!G4VPersistencyManager::GetPersistencyManager()) {
    new PersistencyManager(filename, start_id);
  }
}
