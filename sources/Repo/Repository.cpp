#include "Repository.hpp"

#include "Decoders/UnitDecoder.hpp"
#include "Decoders/XML_Decoder.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

string makeDeviceIdentity(const string& vendor_id, const string& device_id) {
  if (vendor_id.empty()) {
    throw invalid_argument(
        "Failed to create DeviceIDentity string. Vendor ID can not be empty");
  }
  if (device_id.empty()) {
    throw invalid_argument(
        "Failed to create DeviceIDentity string. Device ID can not be empty");
  }
  return vendor_id + "-" + device_id;
}

Repository::Repository(const UnitsMapPtr& units,
    const DatatypesMapPtr& datatypes,
    const VariablesMap& std_variables,
    DescriptorsMap&& descriptors)
    : units_(units), datatypes_(datatypes), std_variables_(std_variables),
      descriptors_(move(descriptors)) {}

Repository::~Repository() {
  descriptors_.clear();
  std_variables_.clear();
  datatypes_.reset();
  units_.reset();
}

DeviceDescriptorPtr Repository::getDescriptor(
    const string& vendor_id, const string& device_id) const {
  auto identity = makeDeviceIdentity(vendor_id, device_id);
  auto it = descriptors_.find(identity);
  if (it != descriptors_.end()) {
    return it->second;
  } else {
    throw runtime_error(
        "Local descriptors repository has no " + identity + " descriptor");
  }
}

DescriptorsMap Repository::getDescriptors() const { return descriptors_; }

size_t Repository::size() const noexcept { return descriptors_.size(); }

RepositoryPtr makeRepository(const filesystem::path& dir) {
  auto std_units_map = decodeUnits(dir / "IODD-StandardUnitDefinitions.xml");
  auto std_variables_map =
      decodeStdDefinitions(dir / "IODD-StandardDefinitions.xml");
  auto descriptors =
      decodeDescriptors(std_units_map, std_variables_map, dir / "iodds");

  return make_unique<Repository>(std_units_map,
      std_variables_map.first,
      std_variables_map.second,
      move(descriptors));
}
} // namespace IODD