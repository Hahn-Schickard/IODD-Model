#include "Repository.hpp"

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

Repository::Repository(UnitsMapPtr&& units,
    DatatypesMapPtr&& datatypes,
    VariablesMapPtr&& std_variables,
    DescriptorsMap&& descriptors)
    : units_(move(units)), datatypes_(move(datatypes)),
      std_variables_(move(std_variables)), descriptors_(descriptors) {}

Repository::Repository(UnitsMapPtr&& units,
    std::pair<DatatypesMapPtr, VariablesMapPtr> std_defines,
    DescriptorsMap&& descriptors)
    : Repository(move(units),
          move(std_defines.first),
          move(std_defines.second),
          move(descriptors)) {}

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

Repository::DescriptorsMap Repository::getDescriptors() const {
  return descriptors_;
}

size_t Repository::size() const noexcept { return descriptors_.size(); }
} // namespace IODD