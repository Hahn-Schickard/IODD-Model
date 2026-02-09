#include "UserInterface.hpp"

using namespace std;

namespace IODD {

string toString(UserRole role) {
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

void addMenu(unordered_map<string, MenuPtr>& menus, const MenuPtr& menu);

void addSubmenu(
    unordered_map<string, MenuPtr>& menus, const Menu::Refs& submenus) {
  for (const auto& submenu : submenus) {
    if (holds_alternative<MenuPtr>(submenu)) {
      addMenu(menus, get<MenuPtr>(submenu));
    }
  }
}

void addMenu(unordered_map<string, MenuPtr>& menus, const MenuPtr& menu) {
  if (menu) {
    menus.emplace(menu->id(), menu);
    if (auto submenus = menu->references(); !submenus.empty()) {
      addSubmenu(menus, submenus);
    }
  }
}

UserInterface::UserInterface(UserRole role,
    const MenuPtr& identification,
    const MenuPtr& parameter,
    const MenuPtr& observation,
    const MenuPtr& diagnosis) :
    role_(role),
    identification_(identification),
    parameter_(parameter),
    observation_(observation),
    diagnosis_(diagnosis) {
  addMenu(menus_, identification_);
  addMenu(menus_, parameter_);
  addMenu(menus_, observation_);
  addMenu(menus_, diagnosis_);
}

UserRole UserInterface::role() const { return role_; }

MenuPtr UserInterface::identification() const { return identification_; }

MenuPtr UserInterface::parameter() const { return parameter_; }

MenuPtr UserInterface::observation() const { return observation_; }

MenuPtr UserInterface::diagnosis() const { return diagnosis_; }

MenuPtr UserInterface::getMenu(const string& menu_id) const {
  if (auto it = menus_.find(menu_id); it != menus_.end()) {
    return it->second;
  }
  throw out_of_range(
      toString(role_) + " does not contain " + menu_id + " Menu");
}

} // namespace IODD
