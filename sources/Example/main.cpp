#include "Serializer.hpp"

#include <iostream>

using namespace std;
using namespace IODD;

inline constexpr uint8_t PADDING_SIZE = 1;
inline constexpr string_view BULLET = "•";

inline string pad(size_t times = 1) {
  return string(times * PADDING_SIZE, ' ');
}

void printUI(const UserInterfacePtr& ui) {
  cout << pad() << toString(ui->role) << " UI has:" << endl;
  cout << pad(2) << BULLET << pad() << ui->parameter->size()
       << " parameter menus" << endl;
  cout << pad(2) << BULLET << pad() << ui->identification->size()
       << " identification menus" << endl;
  cout << pad(2) << BULLET << pad() << ui->observation->size()
       << " observation menus" << endl;
  cout << pad(2) << BULLET << pad() << ui->diagnosis->size()
       << " diagnosis menus" << endl;
}

void printDescriptor(const DeviceDescriptorPtr& descriptor) {
  cout << pad() << "Device " << descriptor->getDeviceName().locale()
       << " is manufactured by " << descriptor->getVendorName() << " has "
       << descriptor->variableCount() << " variables" << endl;

  for (const auto& ui : descriptor->getUIs()) {
    printUI(ui.second);
  }
}

int main() {
  auto repo = deserializeModel("config");

  cout << "IODD Repository has " << repo.size() << " descriptors" << endl;
  for (const auto& descriptor : repo.getDescriptors()) {
    cout << "Device descriptor for " << descriptor.first << " device:" << endl;
    printDescriptor(descriptor.second);
  }

  exit(EXIT_SUCCESS);
}