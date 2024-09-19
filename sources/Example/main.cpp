#include "Repository.hpp"

#include "Variant_Visitor.hpp"

#include <iostream>

using namespace std;
using namespace IODD;

constexpr uint8_t PADDING_SIZE = 1;

string pad(size_t times = 1) { return string(times * PADDING_SIZE, ' '); }

string padDot(size_t times = 1) { return pad(times) + "•" + pad(); }

void printVariable(const VariablePtr& var, const size_t offset) {
  cout << pad(offset) << var->index() << " " << toString(var->type()) << " "
       << var->name().locale();
}

void printCondition(const optional<Condition>& condition, const size_t offset) {
  if (condition) {
    cout << pad(offset) << "Is used if variable: ";
    printVariable(condition->variable(), 0);
    if (auto subindex = condition->subindex()) {
      cout << " subindex " << (size_t)*subindex;
    }
    cout << " is equal to " << (size_t)condition->value() << endl;
  }
}

void printButton(const VariableRefPtr& ref, const size_t offset) {
  cout << pad(offset) << "That is used as a button";
  if (auto desc = ref->description()) {
    cout << " that " << desc->locale();
  }

  if (auto action_msg = ref->actionMessage()) {
    cout << " that " << action_msg->locale();
  }

  cout << endl << pad(offset) << "This button sends";
  match(
      ref->buttonValue(),
      [](bool value) { cout << " boolean value: " << boolalpha << value; },
      [](uint64_t value) { cout << " unsigned value: " << value; },
      [](int64_t value) { cout << " integer value: " << value; });
  cout << " to the Device" << endl;
}

void printRefRepresentation(const VariableRefPtr& ref, const size_t offset) {
  if (ref->isButton()) {
    printButton(ref, offset);
  } else {
    if (auto unit = ref->unit()) {
      cout << pad(offset) << "It measures";
      if (auto name = unit->name()) {
        cout << " " << name->locale();
      }
      auto symbol = unit->abbr();
      cout << " in " << symbol << endl;
    }
    if (isDecimal(ref->displayFormat())) {
      if (isNumericData(ref->variable()->type())) {
        cout << pad(offset)
             << "The measurement is calculated as follows: " << endl;
        cout << pad(offset) << "(Raw Value * " << ref->gradient() << ") + "
             << ref->offset() << endl;
      }
    } else if (isBinary(ref->displayFormat())) {
      cout << pad(offset) << "It is represented as a binary value" << endl;
    } else {
      cout << pad(offset) << "It is represented as a hexadecimal value" << endl;
    }
  }
}

void printVarRef(const VariableRefPtr& ref, const size_t offset) {
  cout << "Variable reference for ";
  printVariable(ref->variable(), 0);
  cout << endl;
  printRefRepresentation(ref, offset);
}

void printRecordRef(const RecordRefPtr& ref, const size_t offset) {
  cout << "RecordItem reference for ";
  printVariable(ref->variable(), 0);
  cout << " subindex " << (size_t)ref->subindex() << endl;
  printRefRepresentation(ref, offset);
}

void printMenu(const MenuPtr& menu, const size_t offset) {
  cout << "Menu id: " << menu->id();
  if (auto name = menu->name()) {
    cout << " " << name->locale();
  }
  cout << " contains:" << endl;
  printCondition(menu->condition(), offset + PADDING_SIZE);
  for (const auto& sub_menu : menu->references()) {
    cout << padDot(offset + PADDING_SIZE) << " ";
    match(
        sub_menu,
        [&offset](const VariableRefPtr& variable_ref) {
          printVarRef(variable_ref, offset + PADDING_SIZE);
        },
        [&offset](const RecordRefPtr& record_ref) {
          printRecordRef(record_ref, offset + PADDING_SIZE);
        },
        [&offset](
            const MenuPtr& menu) { printMenu(menu, offset + PADDING_SIZE); });
  }
}

void printUI(const UserInterfacePtr& ui) {
  cout << pad() << toString(ui->role()) << " UI has:" << endl;

  auto params = ui->parameter();
  cout << padDot(2) << params->size() << " parameter menus" << endl;
  printMenu(params, 2);

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
  try {
    auto repo = makeRepository(filesystem::path("config"));

    cout << "IODD Repository has " << repo->size() << " descriptors" << endl;
    for (const auto& descriptor : repo->getDescriptors()) {
      cout << "Device descriptor for " << descriptor.first
           << " device:" << endl;
      printDescriptor(descriptor.second);
    }

    exit(EXIT_SUCCESS);
  } catch (const exception& ex) {
    cerr << "Encountered an exception: " << ex.what() << endl;
    exit(EXIT_FAILURE);
  }
}