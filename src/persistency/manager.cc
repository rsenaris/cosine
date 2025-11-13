#include "persistency/manager.hh"
#include "G4Exception.hh"
#include "G4ExceptionSeverity.hh"
#include "persistency/hdf5_writer.hh"

std::unique_ptr<PersistencyManager> PERSISTENCY_MANAGER = nullptr;

void PersistencyManager::Initialize(std::string &filename, G4int start_id) {
  if (G4VPersistencyManager::GetPersistencyManager())
    G4Exception("[Persistency::Initialize]", "", FatalException, "Persistency manager initialized twice");

  PERSISTENCY_MANAGER.reset(new PersistencyManager(filename, start_id));
}
