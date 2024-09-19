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

UserInterface::UserInterface(UserRole role,
    const MenuPtr& identification,
    const MenuPtr& parameter,
    const MenuPtr& observation,
    const MenuPtr& diagnosis)
    : role_(role), identification_(identification), parameter_(parameter),
      observation_(observation), diagnosis_(diagnosis) {}

UserRole UserInterface::role() const { return role_; }

MenuPtr UserInterface::identification() const { return identification_; }

MenuPtr UserInterface::parameter() const { return parameter_; }

MenuPtr UserInterface::observation() const { return observation_; }

MenuPtr UserInterface::diagnosis() const { return diagnosis_; }

} // namespace IODD
