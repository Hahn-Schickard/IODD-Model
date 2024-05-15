#ifndef __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP
#define __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP

#include "IODeviceDescriptor.hpp"

#include <string>
#include <unordered_map>

namespace IODD {
struct Repository {
  using DescriptorsMap = std::unordered_map<std::string, IODeviceDescriptorPtr>;
  using VariablesMap = std::unordered_map<std::string, DataValue>;
  using VariablesMapPtr = std::shared_ptr<VariablesMap>;
  using UnitsMap = std::unordered_map<uint16_t, Unit>;
  using UnitsMapPtr = std::shared_ptr<UnitsMap>;

  Repository(DescriptorsMap&& descriptors, UnitsMap&& units,
      VariablesMap&& std_variables);

  IODeviceDescriptorPtr getDescriptor(
      const std::string& vendor_id, const std::string& device_id);

private:
  DescriptorsMap descriptors_;
  UnitsMapPtr units_;
  VariablesMapPtr std_variables_;
};

} // namespace IODD

#endif //__IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP