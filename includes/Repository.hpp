#ifndef __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP
#define __IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP

#include "IODevice.hpp"

#include <string>
#include <unordered_map>

namespace IODD {
struct Repository {
  Repository getDescriptor(
      const std::string& vendor_id, const std::string& device_id);

  std::string getDeviceIdentity(
      const std::string& vendor_id, const std::string& device_id);

  void operator+=(const IODevice& descriptor);

private:
  std::unordered_map<std::string, IODevice> descriptors_;
};

} // namespace IODD

#endif //__IODD_LOCAL_DESCRIPTOR_REPOSITORY_HPP