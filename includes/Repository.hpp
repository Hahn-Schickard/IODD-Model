#ifndef __IODD_DESCRIPTOR_REPOSITORY_HPP
#define __IODD_DESCRIPTOR_REPOSITORY_HPP

#include "Model/DeviceDescriptor.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace IODD {
struct Repository {
  Repository(UnitsMapPtr&& units,
      DatatypesMapPtr&& datatypes,
      VariablesMapPtr&& std_variables,
      DescriptorsMap&& descriptors);

  Repository(UnitsMapPtr&& units,
      std::pair<DatatypesMapPtr, VariablesMapPtr> std_defines,
      DescriptorsMap&& descriptors);

  DeviceDescriptorPtr getDescriptor(
      const std::string& vendor_id, const std::string& device_id) const;

  DescriptorsMap getDescriptors() const;

  size_t size() const noexcept;

private:
  UnitsMapPtr units_;
  DatatypesMapPtr datatypes_;
  VariablesMapPtr std_variables_;
  DescriptorsMap descriptors_;
};

using RepositoryPtr = std::unique_ptr<Repository>;

RepositoryPtr makeRepository(const std::filesystem::path& dir);

} // namespace IODD

#endif //__IODD_DESCRIPTOR_REPOSITORY_HPP