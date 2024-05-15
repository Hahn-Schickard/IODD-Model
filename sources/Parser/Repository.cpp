#include "Repository.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

Repository::Repository(DescriptorsMap&& descriptors)
    : descriptors_(descriptors) {}

IODeviceDescriptorPtr Repository::getDescriptor(
    const string& vendor_id, const string& device_id) {
  auto identity = makeDeviceIdentity(vendor_id, device_id);
  auto it = descriptors_.find(identity);
  if (it != descriptors_.end()) {
    return it->second;
  } else {
    throw runtime_error(
        "Local descriptors repository has no " + identity + " descriptor");
  }
}
} // namespace IODD