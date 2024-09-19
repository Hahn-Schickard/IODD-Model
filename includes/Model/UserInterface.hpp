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

std::string toString(UserRole role);

struct UserInterface {
  UserInterface(UserRole role,
      const MenuPtr& identification,
      const MenuPtr& parameter,
      const MenuPtr& observation,
      const MenuPtr& diagnosis);

  UserRole role() const;
  MenuPtr identification() const;
  MenuPtr parameter() const;
  MenuPtr observation() const;
  MenuPtr diagnosis() const;

private:
  UserRole role_;
  MenuPtr identification_;
  MenuPtr parameter_;
  MenuPtr observation_;
  MenuPtr diagnosis_;
};

using UserInterfacePtr = std::shared_ptr<UserInterface>;

} // namespace IODD

#endif // __IODD_STANDARD_USER_INTERFACE_HPP