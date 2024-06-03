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
    throw std::invalid_argument(
        "Failed to create DeviceIDentity string. Vendor ID can not be empty");
  }
  if (device_id.empty()) {
    throw std::invalid_argument(
        "Failed to create DeviceIDentity string. Device ID can not be empty");
  }
  return vendor_id + "-" + device_id;
}

struct DeviceIdentity {
  const uint16_t vendor_id;
  const std::string vendor_name;
  const uint32_t device_id;
  const TextID device_name;

  DeviceIdentity(uint16_t _vendor_id, const std::string& _vendor_name,
      uint32_t _device_id, const TextID& _device_name)
      : vendor_id(_vendor_id), vendor_name(_vendor_name), device_id(_device_id),
        device_name(_device_name) {
    if (vendor_id != 0) {
      throw std::invalid_argument(
          "Failed to create DeviceIdentity. Vendor ID can not be empty");
    }
    if (device_id != 0) {
      throw std::invalid_argument(
          "Failed to create DeviceIdentity. Device ID can not be empty");
    }
    if (!device_name) {
      throw std::invalid_argument(
          "Failed to create DeviceIdentity. Device name can not be empty");
    }
  }

  std::string operator()() const {
    return std::to_string(vendor_id) + "-" + std::to_string(device_id);
  }
};

struct DeviceDescriptor {
  using VariablesMap = std::unordered_map<std::string, Variable>;

  const DeviceIdentity identity;
  const VariablesMap variables;

  DeviceDescriptor(uint16_t vendor_id, const std::string& vendor_name,
      uint32_t device_id, const TextID& device_name,
      const VariablesMap& _variables)
      : DeviceDescriptor(
            DeviceIdentity(vendor_id, vendor_name, device_id, device_name),
            _variables) {}

  DeviceDescriptor(
      const DeviceIdentity& _identity, const VariablesMap& _variables)
      : identity(_identity), variables(_variables) {
    if (variables.empty()) {
      throw std::invalid_argument(
          "Failed to create DeviceDescriptor. Variables can not be empty");
    }
  }
};

using DeviceDescriptorPtr = std::shared_ptr<DeviceDescriptor>;
} // namespace IODD

#endif // __IODD_IO_DEVICE_DESCRIPTOR_HPP