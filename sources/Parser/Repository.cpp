#include "Repository.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

string Repository::getDeviceIdentity(
    const string& vendor_id, const string& device_id) {
  if (vendor_id.empty()) {
    throw invalid_argument("Vendor ID can not be empty");
  }
  if (device_id.empty()) {
    throw invalid_argument("Device ID can not be empty");
  }
  return vendor_id + "-" + device_id;
}

void Repository::operator+=(const IODevice& descriptor) {
  descriptors_.emplace(
      getDeviceIdentity(descriptor.vendor_id, descriptor.device_id),
      descriptor);
}

IODevice Repository::getDescriptor(
    const string& vendor_id, const string& device_id) {
  auto identity = getDeviceIdentity(vendor_id, device_id);
  auto it = descriptors_.find(identity);
  if (it != descriptors_.end()) {
    return it->second;
  } else {
    throw runtime_error(
        "Local descriptors repository has no " + identity + " descriptor");
  }
}
} // namespace IODD