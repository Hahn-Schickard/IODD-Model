#include "Serializer.hpp"

#include <iostream>

using namespace std;
using namespace IODD;

constexpr uint8_t PADDING_SIZE = 1;

string pad(size_t times = 1) { return string(times * PADDING_SIZE, ' '); }

string padDot(size_t times = 1) { return pad(times) + "•" + pad(); }

void printUI(const UserInterfacePtr& ui) {
  cout << pad() << toString(ui->role()) << " UI has:" << endl;
  cout << padDot(2) << ui->parameter()->size() << " parameter menus" << endl;
  cout << padDot(2) << ui->identification()->size() << " identification menus"
       << endl;
  cout << padDot(2) << ui->observation()->size() << " observation menus"
       << endl;
  cout << padDot(2) << ui->diagnosis()->size() << " diagnosis menus" << endl;
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