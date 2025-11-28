#include "common.hh"
#include "persistency/hdf5_writer.hh"
#include "config.hh"
#include "persistency/hdf5_types.hh"

#include <G4ThreeVector.hh>

#include "highfive/H5PropertyList.hpp"
#include "types.hh"
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5DataType.hpp>
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>

#include <cstring>
#include <memory>
#include <stdexcept>

using namespace HighFive;


HDF5Writer::HDF5Writer(std::string& filename)
    : vol_change_writer_(nullptr)
    , sens_writer_      (nullptr)
    , file_             (nullptr)
    , filename_         (filename)
{}

HDF5Writer::~HDF5Writer() {
  vol_change_writer_.reset();
  sens_writer_      .reset();
  file_.reset();
}

void HDF5Writer::write_steps(std::vector<VolumeChange>&& steps) {
  if (!vol_change_writer_) {
    auto dataset = create_dataset("MC", "volume_changes", create_volume_change(), LARGE_CHUNK_SIZE, false);
    vol_change_writer_ = std::make_unique<BufferedWriter<VolumeChange>>(std::move(dataset), LARGE_CHUNK_SIZE);
  }
  vol_change_writer_ -> write(std::move(steps));
}

void HDF5Writer::write_hits(std::vector<SensorHit>&& hits) {
  if (!sens_writer_) {
    auto dataset = create_dataset("MC", "sensor_hits", create_sensor_hit(), LARGE_CHUNK_SIZE, false);
    sens_writer_ = std::make_unique<BufferedWriter<SensorHit>>(std::move(dataset), LARGE_CHUNK_SIZE);
  }
  sens_writer_ -> write(std::move(hits));
}

void HDF5Writer::write_interaction(Interaction&& intrs) {
  if (!interaction_writer_) {
    auto dataset = create_dataset("MC", "interactions", create_interaction(), LARGE_CHUNK_SIZE, false);
    interaction_writer_ = std::make_unique<BufferedWriter<Interaction>>(std::move(dataset), LARGE_CHUNK_SIZE);
  }
  interaction_writer_->write(std::move(intrs));
}

void HDF5Writer::write_config(std::vector<ConfPar>&& confs) {
  if (!config_writer_) {
    auto dataset = create_dataset("MC", "config", create_config(), LARGE_CHUNK_SIZE, false);
    config_writer_ = std::make_unique<BufferedWriter<ConfPar>>(std::move(dataset), LARGE_CHUNK_SIZE);
  }
  config_writer_ -> write(std::move(confs));
}

DataSet HDF5Writer::create_dataset( std::string  const& group_name
                                  , std::string  const&  node_name
                                  , CompoundType const& type
                                  , hsize_t             chunk_size
                                  , bool                deflate
                                  ) {
  if (!file_) open_file();

  auto group =
    file_ -> exist      (group_name) ?
    file_ -> getGroup   (group_name) :
    file_ -> createGroup(group_name) ;

  if (group.exist(node_name))
    throw std::runtime_error("Node " + node_name + " already exists");

  // To create a table than can be resized it has be of UNLIMITED dimension
  // and requires chunking of the data
  auto extensible_ds = DataSpace({0}, {DataSpace::UNLIMITED});

  DataSetCreateProps ds_props;
  ds_props.add(HighFive::Chunking(std::vector<hsize_t>{chunk_size}));
  if (deflate) ds_props.add(HighFive::Deflate(4));
  auto dataset = group.createDataSet(node_name, extensible_ds, type, ds_props);
  return dataset;
}

void HDF5Writer::open_file() {
  auto options = File::Overwrite;
  file_ = std::make_unique<File>(filename_, options);
}
