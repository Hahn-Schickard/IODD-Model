#ifndef __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP
#define __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP

#include "IODeviceDescriptor.hpp"

#include <string>
#include <unordered_map>

namespace IODD {
struct Repository {
  using UnitsMap = std::unordered_map<uint16_t, Unit>;
  using UnitsMapPtr = std::shared_ptr<UnitsMap>;
  using DatatypesMap = std::unordered_map<std::string, DataValue>;
  using DatatypesMapPtr = std::shared_ptr<DatatypesMap>;
  using VariablesMap = std::unordered_map<std::string, Variable>;
  using VariablesMapPtr = std::shared_ptr<VariablesMap>;
  using DescriptorsMap = std::unordered_map<std::string, IODeviceDescriptorPtr>;

  Repository(UnitsMap&& units, DatatypesMap&& datatypes,
      VariablesMap&& std_variables, DescriptorsMap&& descriptors);

  Repository(UnitsMap&& units,
      std::pair<DatatypesMap, VariablesMap> std_defines,
      DescriptorsMap&& descriptors);

  IODeviceDescriptorPtr getDescriptor(
      const std::string& vendor_id, const std::string& device_id);

private:
  UnitsMapPtr units_;
  DatatypesMapPtr datatypes_;
  VariablesMapPtr std_variables_;
  DescriptorsMap descriptors_;
};

} // namespace IODD

#endif //__IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP