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

struct DeviceIdentity {
  const std::string vendor_id;
  const std::string vendor_name;
  const std::string device_id;

  DeviceIdentity(const std::string& _vendor_id, const std::string& _vendor_name,
      const std::string& _device_id) {
    if (vendor_id.empty()) {
      throw std::invalid_argument("Vendor ID can not be empty");
    }
    if (device_id.empty()) {
      throw std::invalid_argument("Device ID can not be empty");
    }
  }

  std::string operator()() { return makeDeviceIdentity(vendor_id, device_id); }
};

struct DeviceDescriptor {
  using VariablesMap = std::unordered_map<std::string, Variable>;

  const DeviceIdentity identity;
  const VariablesMap variables;

  DeviceDescriptor(const std::string& vendor_id, const std::string& vendor_name,
      const std::string& device_id, const VariablesMap& _variables)
      : identity(vendor_id, vendor_name, device_id), variables(_variables) {
    if (variables.empty()) {
      throw std::invalid_argument("Variables can not be empty");
    }
  }
};

using DeviceDescriptorPtr = std::shared_ptr<DeviceDescriptor>;
} // namespace IODD

#endif // __IODD_IO_DEVICE_DESCRIPTOR_HPP