#include "Repository.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

Repository::Repository(DescriptorsMap&& descriptors, UnitsMap&& units,
    VariablesMap&& std_variables)
    : descriptors_(descriptors), units_(make_shared<UnitsMap>(units)),
      std_variables_(make_shared<VariablesMap>(std_variables)) {}

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