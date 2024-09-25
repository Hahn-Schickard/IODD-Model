#ifndef __IODD_STANDARD_DEVICE_DESCRIPTOR_HPP
#define __IODD_STANDARD_DEVICE_DESCRIPTOR_HPP

#include "ProcessDataUnion.hpp"
#include "Unit.hpp"
#include "UserInterface.hpp"
#include "Variable.hpp"

namespace IODD {

struct DeviceIdentity {
  DeviceIdentity(uint16_t vendor_id,
      const std::string& vendor_name,
      uint32_t device_id,
      const TextID& device_name);

  std::string getIdentifier() const;

  uint16_t getVendorId() const;

  std::string getVendorName() const;

  uint32_t getDeviceId() const;

  TextID getDeviceName() const;

private:
  uint16_t vendor_id_;
  std::string vendor_name_;
  uint32_t device_id_;
  TextID device_name_;
};

struct DeviceDescriptor : public DeviceIdentity {
  using UserInterfaces = std::unordered_map<UserRole, UserInterfacePtr>;

  DeviceDescriptor(uint16_t vendor_id,
      const std::string& vendor_name,
      uint32_t device_id,
      const TextID& device_name,
      const UnitsMapPtr& units,
      VariablesMap&& variables,
      ProcessDataCollection&& process_data,
      UserInterfaces&& interfaces);

  DeviceDescriptor(DeviceIdentity&& identity,
      const UnitsMapPtr& units,
      VariablesMap&& variables,
      ProcessDataCollection&& process_data,
      UserInterfaces&& interfaces);

  VariablePtr getVariable(const std::string& id) const;

  VariablesMap getVariables() const;

  size_t variableCount() const;

  NamedAttributePtr getVariableValueName(const std::string& id,
      const SimpleDatatypeValue& value,
      std::optional<uint8_t> subindex = std::nullopt) const;

  UserInterfaces getUIs() const;

  UserInterfacePtr getObserverUI() const;

  UserInterfacePtr getMaintainenceUI() const;

  UserInterfacePtr getSpecialistUI() const;

private:
  UnitsMapPtr units_;
  VariablesMap variables_;
  ProcessDataCollection process_data_;
  UserInterfaces interfaces_;
};

using DeviceDescriptorPtr = std::shared_ptr<DeviceDescriptor>;
using DescriptorsMap = std::unordered_map<std::string, DeviceDescriptorPtr>;
} // namespace IODD

#endif // __IODD_STANDARD_DEVICE_DESCRIPTOR_HPP
