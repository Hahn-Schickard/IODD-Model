#include "DeviceDescriptor.hpp"

using namespace std;

namespace IODD {

DeviceIdentity::DeviceIdentity(uint16_t vendor_id,
    const string& vendor_name,
    uint32_t device_id,
    const TextIDPtr& device_name)
    : vendor_id_(vendor_id), vendor_name_(vendor_name), device_id_(device_id),
      device_name_(device_name) {
  if (!device_name_) {
    throw invalid_argument(
        "Failed to create DeviceIdentity. Device name can not be empty");
  }
}

DeviceIdentity::DeviceIdentity(DeviceIdentity&& other) noexcept {
  swap(vendor_id_, other.vendor_id_);
  swap(vendor_name_, other.vendor_name_);
  swap(device_id_, other.device_id_);
  swap(device_name_, other.device_name_);
}

string DeviceIdentity::getIdentifier() const {
  return to_string(vendor_id_) + "-" + to_string(device_id_);
}

uint16_t DeviceIdentity::getVendorId() const { return vendor_id_; }

string DeviceIdentity::getVendorName() const { return vendor_name_; }

uint32_t DeviceIdentity::getDeviceId() const { return device_id_; }

TextIDPtr DeviceIdentity::getDeviceName() const { return device_name_; }

DeviceDescriptor::DeviceDescriptor(uint16_t vendor_id,
    const string& vendor_name,
    uint32_t device_id,
    const TextIDPtr& device_name,
    const UnitsMapPtr& units,
    VariablesMap&& variables,
    ProcessDataCollection&& process_data,
    UserInterfaces&& interfaces)
    : DeviceDescriptor(
          DeviceIdentity(vendor_id, vendor_name, device_id, device_name),
          units,
          move(variables),
          move(process_data),
          move(interfaces)) {}

DeviceDescriptor::DeviceDescriptor(DeviceIdentity&& identity,
    const UnitsMapPtr& units,
    VariablesMap&& variables,
    ProcessDataCollection&& process_data,
    UserInterfaces&& interfaces)
    : DeviceIdentity(move(identity)), units_(units),
      variables_(move(variables)), process_data_(move(process_data)),
      interfaces_(move(interfaces)) {
  if (units_->empty()) {
    throw invalid_argument(
        "Failed to create DeviceDescriptor. Units can not be empty");
  }
  if (variables_.empty()) {
    throw invalid_argument(
        "Failed to create DeviceDescriptor. Variables can not be empty");
  }
  if (interfaces_.empty()) {
    throw invalid_argument(
        "Failed to create DeviceDescriptor. UserInterfaces can not be empty");
  }
}

VariablePtr DeviceDescriptor::getVariable(const string& id) const {
  return findVariable(id, variables_);
}

VariablesMap DeviceDescriptor::getVariables() const { return variables_; }

size_t DeviceDescriptor::variableCount() const { return variables_.size(); }

NamedAttributePtr DeviceDescriptor::getVariableValueName(const string& id,
    const SimpleDatatypeValue& value,
    optional<uint8_t> subindex) const {
  auto variable = getVariable(id);
  return variable->valueName(value, subindex);
}

DeviceDescriptor::UserInterfaces DeviceDescriptor::getUIs() const {
  return interfaces_;
}

UserInterfacePtr DeviceDescriptor::getObserverUI() const {
  return interfaces_.at(UserRole::ObservationRole);
}

UserInterfacePtr DeviceDescriptor::getMaintainenceUI() const {
  return interfaces_.at(UserRole::MaintenanceRole);
}

UserInterfacePtr DeviceDescriptor::getSpecialistUI() const {
  return interfaces_.at(UserRole::SpecialistRole);
}
} // namespace IODD
