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

  void write(std::vector<DATA>&& items) {
    for (auto item : std::move(items)) {
      buffer_.push_back(std::move(item));
    }
    flush();
  }

private:
  HighFive::DataSet dataset_;
  size_t            buffer_size_;
  std::vector<DATA> buffer_;

  void flush() {
    if (buffer_.empty()) return;

    auto n       = buffer_.size();
    auto current = dataset_.getDimensions()[0];
    dataset_.resize({current +  n});
    dataset_.select({current}, {n}).write(buffer_);
    buffer_.clear();
  }
};

class HDF5Writer {
public:
  HDF5Writer(const std::string& filename, G4int start_event);
  ~HDF5Writer();

  void write_steps(std::vector<StepData >&& steps);
  void write_hits (std::vector<SensorHit>&&  hits);

private:
  std::unique_ptr<BufferedWriter<StepData >> step_writer_;
  std::unique_ptr<BufferedWriter<SensorHit>> sens_writer_;
  std::unique_ptr<HighFive::File>            file_;

  HighFive::DataSet create_dataset( std::string            const& group_name
                                  , std::string            const&  node_name
                                  , HighFive::CompoundType const& type
                                  , hsize_t                       chunk_size);
};
