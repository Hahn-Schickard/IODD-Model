#ifndef __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP
#define __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP

#include "IODeviceDescriptor.hpp"

#include <string>
#include <unordered_map>

namespace IODD {
struct Repository {
  using DescriptorsMap = std::unordered_map<std::string, IODeviceDescriptorPtr>;

  Repository(DescriptorsMap&& descriptors);

  IODeviceDescriptorPtr getDescriptor(
      const std::string& vendor_id, const std::string& device_id);

private:
  DescriptorsMap descriptors_;
};

} // namespace IODD

#endif //__IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP