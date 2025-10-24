#pragma once

#include "common.hh"
#include  "types.hh"

#include <G4Types.hh>
#include <G4VPersistencyManager.hh>

#include <highfive/H5DataSet.hpp>
#include <highfive/H5File.hpp>


template<class DATA>
class BufferedWriter {
public:
  BufferedWriter(HighFive::DataSet&& dataset, size_t buffer_size)
      : dataset_(dataset)
      , buffer_size_(buffer_size)
      , buffer_()
  {}

  ~BufferedWriter() { flush(); }

  void operator()(DATA&& item) {
    buffer_.push_back(std::move(item));
    if (buffer_.size() >= buffer_size_) { flush(); }
  }

  void operator()(std::vector<DATA>&& items) {
    for (auto item : std::move(items)) {
      (*this)(std::move(item));
    }
  }

private:
  HighFive::DataSet dataset_;
  size_t            buffer_size_;
  std::vector<DATA> buffer_;

  void flush() {
    if (buffer_.empty()) { return; }
    auto n       = buffer_.size();
    auto current = dataset_.getDimensions()[0];
    dataset_.resize({current +  n});
    dataset_.select({current}, {n}).write(buffer_);
    buffer_.clear();
  }
};

class HDF5Writer : public G4VPersistencyManager {
public:
  HDF5Writer(G4String filename, G4int start_event);
  ~HDF5Writer(){}

  void  OpenFile(){ open_file();}
  void CloseFile(){close_file();}

  void store_steps(std::vector<StepData>&& steps);
  void next_event(){current_event_++;}

private:
  std::string               filename_;
  HighFive::File*           file_;
  BufferedWriter<StepData>* step_writer_;
  G4int                     current_event_;

  std::string label(const std::vector<std::string> tokens) const;

  void  open_file();
  void close_file();
  HighFive::DataSet create_dataset( std::string            const& group_name
                                  , std::string            const&  node_name
                                  , HighFive::CompoundType const& type
                                  , hsize_t                       chunk_size);
};
