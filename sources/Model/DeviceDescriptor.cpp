#include "DeviceDescriptor.hpp"

using namespace std;

namespace IODD {

DeviceIdentity::DeviceIdentity(uint16_t vendor_id,
    const string& vendor_name,
    uint32_t device_id,
    const TextID& device_name)
    : vendor_id_(vendor_id), vendor_name_(vendor_name), device_id_(device_id),
      device_name_(device_name) {
  if (!device_name_) {
    throw invalid_argument(
        "Failed to create DeviceIdentity. Device name can not be empty");
  }
}

string DeviceIdentity::getIdentifier() const {
  return to_string(vendor_id_) + "-" + to_string(device_id_);
}

uint16_t DeviceIdentity::getVendorId() const { return vendor_id_; }

string DeviceIdentity::getVendorName() const { return vendor_name_; }

uint32_t DeviceIdentity::getDeviceId() const { return device_id_; }

TextID DeviceIdentity::getDeviceName() const { return device_name_; }

DeviceDescriptor::DeviceDescriptor(uint16_t vendor_id,
    const string& vendor_name,
    uint32_t device_id,
    const TextID& device_name,
    const UnitsMapPtr& units,
    const VariablesMapPtr& std_variables,
    VariablesMapPtr&& variables,
    UserInterfaces&& interfaces)
    : DeviceDescriptor(
          DeviceIdentity(vendor_id, vendor_name, device_id, device_name),
          units,
          std_variables,
          move(variables),
          move(interfaces)) {}

DeviceDescriptor::DeviceDescriptor(DeviceIdentity&& identity,
    const UnitsMapPtr& units,
    const VariablesMapPtr& std_variables,
    VariablesMapPtr&& variables,
    UserInterfaces&& interfaces)
    : DeviceIdentity(move(identity)), units_(units),
      std_variables_(std_variables), variables_(move(variables)),
      interfaces_(move(interfaces)) {
  if (units_->empty()) {
    throw invalid_argument(
        "Failed to create DeviceDescriptor. Units can not be empty");
  }
  if (std_variables_->empty()) {
    throw invalid_argument("Failed to create DeviceDescriptor. Standard "
                           "Variables can not be empty");
  }
  if (variables_->empty()) {
    throw invalid_argument(
        "Failed to create DeviceDescriptor. Variables can not be empty");
  }
  if (interfaces_.empty()) {
    throw invalid_argument(
        "Failed to create DeviceDescriptor. UserInterfaces can not be empty");
  }
}

VariablePtr DeviceDescriptor::getVariable(const string& id) const {
  if (auto iter = variables_->find(id); iter != variables_->end()) {
    return iter->second;
  } else if (auto iter = std_variables_->find(id);
             iter != std_variables_->end()) {
    return iter->second;
  }
  throw out_of_range("Variable with id " + id + " does not exists");
}

VariablesMapPtr DeviceDescriptor::getSTDVariables() const {
  return std_variables_;
}

VariablesMapPtr DeviceDescriptor::getVariables() const { return variables_; }

size_t DeviceDescriptor::variableCount() const {
  return variables_->size() + std_variables_->size();
}

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
