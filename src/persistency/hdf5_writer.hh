#pragma once

#include "core/common.hh"
#include "core/config.hh"
#include "core/types.hh"

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

  void write(DATA&& item) {
    buffer_.push_back(std::move(item));
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

/// Important!!!
/// The input string must be non-const references in order to allow them to
/// change from macro or CLI

class HDF5Writer {
public:
  HDF5Writer(std::string& filename);
  ~HDF5Writer();

  void write_steps (std::vector<VolumeChange >&& steps);
  void write_sens  (std::vector<SensorHit    >&& sens);
  void write_tracks(std::vector<Track        >&& tracks);
  void write_ihits (std::vector<IonizationHit>&& ihits);
  void write_source(                   Source && sources);
  void write_config(std::vector<ConfPar      >&& confs);

private:
  std::unique_ptr<BufferedWriter<VolumeChange>>  vol_change_writer_;
  std::unique_ptr<BufferedWriter<SensorHit>>           sens_writer_;
  std::unique_ptr<BufferedWriter<Track>>              track_writer_;
  std::unique_ptr<BufferedWriter<IonizationHit>>       ihit_writer_;
  std::unique_ptr<BufferedWriter<Source>>            source_writer_;
  std::unique_ptr<BufferedWriter<ConfPar>>           config_writer_;

  std::unique_ptr<HighFive::File> file_;
  std::string&                    filename_;

  HighFive::DataSet create_dataset( std::string            const& group_name
                                  , std::string            const&  node_name
                                  , HighFive::CompoundType const& type
                                  , hsize_t                       chunk_size
                                  , bool                          deflate
                                  );

  void open_file();
};
