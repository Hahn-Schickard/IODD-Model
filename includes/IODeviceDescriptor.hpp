#ifndef __IODD_IO_DEVICE_DESCRIPTOR_HPP
#define __IODD_IO_DEVICE_DESCRIPTOR_HPP

#include "StandardDefines.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace IODD {

inline std::string makeDeviceIdentity(
    const std::string& vendor_id, const std::string& device_id) {
  if (vendor_id.empty()) {
    throw std::invalid_argument("Vendor ID can not be empty");
  }
  if (device_id.empty()) {
    throw std::invalid_argument("Device ID can not be empty");
  }
  return vendor_id + "-" + device_id;
}

struct IODeviceDescriptor {
  const std::string vendor_id;
  const std::string vendor_name;
  const std::string device_id;

  const std::unordered_map<std::string, DataValue> variables;

  IODeviceDescriptor(const std::string& _vendor_id,
      const std::string& _vendor_name, const std::string& _device_id,
      const std::unordered_map<std::string, DataValue>& _variables)
      : vendor_id(_vendor_id), vendor_name(_vendor_name), device_id(_device_id),
        variables(_variables) {
    if (vendor_id.empty()) {
      throw std::invalid_argument("Vendor ID can not be empty");
    }
    if (device_id.empty()) {
      throw std::invalid_argument("Device ID can not be empty");
    }
    if (variables.empty()) {
      throw std::invalid_argument("Variables can not be empty");
    }
  }

  std::string getDeviceIdentity() {
    return makeDeviceIdentity(vendor_id, device_id);
  }
};

using IODeviceDescriptorPtr = std::shared_ptr<IODeviceDescriptor>;
} // namespace IODD

#endif // __IODD_IO_DEVICE_DESCRIPTOR_HPP