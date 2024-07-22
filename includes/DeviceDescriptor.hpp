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
  DeviceIdentity(uint16_t vendor_id, const std::string& vendor_name,
      uint32_t device_id, const TextID& device_name)
      : vendor_id_(vendor_id), vendor_name_(vendor_name), device_id_(device_id),
        device_name_(device_name) {
    if (vendor_id_ != 0) {
      throw std::invalid_argument(
          "Failed to create DeviceIdentity. Vendor ID can not be empty");
    }
    if (device_id_ != 0) {
      throw std::invalid_argument(
          "Failed to create DeviceIdentity. Device ID can not be empty");
    }
    if (!device_name_) {
      throw std::invalid_argument(
          "Failed to create DeviceIdentity. Device name can not be empty");
    }
  }

  std::string getIdentifier() const {
    return std::to_string(vendor_id_) + "-" + std::to_string(device_id_);
  }

  uint16_t getVendorId() const { return vendor_id_; }

  std::string getVendorName() const { return vendor_name_; }

  uint32_t getDeviceId() const { return device_id_; }

  TextID getDeviceName() const { return device_name_; }

private:
  uint16_t vendor_id_;
  std::string vendor_name_;
  uint32_t device_id_;
  TextID device_name_;
};

struct DeviceDescriptor : public DeviceIdentity {
  using UnitsMap = std::unordered_map<uint16_t, UnitPtr>;
  using UnitsMapPtr = std::shared_ptr<UnitsMap>;
  using VariablesMap = std::unordered_map<std::string, VariablePtr>;
  using VariablesMapPtr = std::shared_ptr<VariablesMap>;

  DeviceDescriptor(uint16_t vendor_id, const std::string& vendor_name,
      uint32_t device_id, const TextID& device_name, const UnitsMapPtr& units,
      const VariablesMapPtr& std_variables, VariablesMap&& variables)
      : DeviceDescriptor(
            DeviceIdentity(vendor_id, vendor_name, device_id, device_name),
            units, std_variables, std::move(variables)) {}

  DeviceDescriptor(DeviceIdentity&& identity, const UnitsMapPtr units,
      const VariablesMapPtr& std_variables, VariablesMap&& variables)
      : DeviceIdentity(std::move(identity)), units_(units),
        std_variables_(std_variables), variables_(std::move(variables)) {
    if (units_->empty()) {
      throw std::invalid_argument(
          "Failed to create DeviceDescriptor. Units can not be empty");
    }
    if (std_variables_->empty()) {
      throw std::invalid_argument("Failed to create DeviceDescriptor. Standard "
                                  "Variables can not be empty");
    }
    if (variables_.empty()) {
      throw std::invalid_argument(
          "Failed to create DeviceDescriptor. Variables can not be empty");
    }
  }

  VariablePtr getVariable(const std::string& id) {
    if (auto iter = variables_.find(id); iter != variables_.end()) {
      return iter->second;
    } else if (auto iter = std_variables_->find(id);
               iter != std_variables_->end()) {
      return iter->second;
    }
    throw std::out_of_range("Variable with id " + id + " does not exists");
  }

  NamedAttributePtr getVariableValueName(const std::string& id, bool value) {}

  NamedAttributePtr getVariableValueName(const std::string& id, size_t value) {}

  NamedAttributePtr getVariableValueName(const std::string& id, float value) {}

  NamedAttributePtr getVariableValueName(
      const std::string& id, uint8_t subindex, bool value) {}

  NamedAttributePtr getVariableValueName(
      const std::string& id, uint8_t subindex, size_t value) {}

  NamedAttributePtr getVariableValueName(
      const std::string& id, uint8_t subindex, float value) {}

private:
  UnitsMapPtr units_;
  VariablesMapPtr std_variables_;
  VariablesMap variables_;
};

using DeviceDescriptorPtr = std::shared_ptr<DeviceDescriptor>;
} // namespace IODD

#endif // __IODD_IO_DEVICE_DESCRIPTOR_HPP