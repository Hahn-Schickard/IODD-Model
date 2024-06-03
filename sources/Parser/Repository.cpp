#include "Repository.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

Repository::Repository(UnitsMap&& units, DatatypesMap&& datatypes,
    VariablesMap&& std_variables, DescriptorsMap&& descriptors)
    : units_(make_shared<UnitsMap>(units)),
      datatypes_(make_shared<DatatypesMap>(datatypes)),
      std_variables_(make_shared<VariablesMap>(std_variables)),
      descriptors_(descriptors) {}

Repository::Repository(UnitsMap&& units,
    std::pair<DatatypesMap, VariablesMap> std_defines,
    DescriptorsMap&& descriptors)
    : Repository(move(units), move(std_defines.first), move(std_defines.second),
          move(descriptors)) {}

DeviceDescriptorPtr Repository::getDescriptor(
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