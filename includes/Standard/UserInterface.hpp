#ifndef __IODD_STANDARD_USER_INTERFACE_HPP
#define __IODD_STANDARD_USER_INTERFACE_HPP

#include "Menu.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace IODD {

enum class UserRole : uint8_t {
  ObservationRole,
  MaintenanceRole,
  SpecialistRole
};

inline std::string toString(UserRole role) {
  switch (role) {
  case UserRole::ObservationRole: {
    return "Observation Role";
  }
  case UserRole::MaintenanceRole: {
    return "Maintenance Role";
  }
  case UserRole::SpecialistRole: {
    return "Specialist Role";
  }
  default: {
    throw std::logic_error("Could not decode UserRole. Unsupported enum value");
  }
  }
}

struct UserInterface {
  UserInterface(UserRole _role,
      MenuPtr _identification,
      MenuPtr _parameter,
      MenuPtr _observation,
      MenuPtr _diagnosis)
      : role(_role), identification(_identification), parameter(_parameter),
        observation(_observation), diagnosis(_diagnosis) {}

  const UserRole role;
  const MenuPtr identification;
  const MenuPtr parameter;
  const MenuPtr observation;
  const MenuPtr diagnosis;
};

using UserInterfacePtr = std::shared_ptr<UserInterface>;

} // namespace IODD

#endif // __IODD_STANDARD_USER_INTERFACE_HPP