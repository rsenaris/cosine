#include "common.hh"
#include "persistency/hdf5_writer.hh"
#include "persistency/hdf5_types.hh"

#include <G4ThreeVector.hh>

#include "highfive/H5PropertyList.hpp"
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5DataType.hpp>
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>

#include <cstring>
#include <stdexcept>

using namespace HighFive;


HDF5Writer::HDF5Writer(const std::string& filename, G4int start_event)
    : filename_(filename)
    , file_(nullptr)
    , step_writer_(nullptr)
{
  open_file();
}

void HDF5Writer::write_steps(std::vector<StepData>&& steps) {
  if (!step_writer_) {
    auto dataset = create_dataset("MC", "steps", create_step_data(), LARGE_CHUNK_SIZE);
    step_writer_ = new BufferedWriter<StepData>{std::move(dataset), LARGE_CHUNK_SIZE};
  }
  step_writer_ -> write(std::move(steps));
}

void HDF5Writer::open_file() {
  file_ = new File{ filename_
                  ,   File::ReadWrite
                    | File::Create
                    | File::Truncate
                  };
}

void HDF5Writer::close_file() {
  file_ -> flush();
}

DataSet HDF5Writer::create_dataset( std::string  const& group_name
                                  , std::string  const&  node_name
                                  , CompoundType const& type
                                  , hsize_t             chunk_size) {
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
  auto dataset = group.createDataSet(node_name, extensible_ds, type, ds_props);
  return dataset;
}
