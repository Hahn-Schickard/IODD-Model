#include <IODD/Model/DeviceDescriptor.hpp>
#include <IODD/Repository.hpp>

#include <iostream>

using namespace std;
using namespace IODD;

int main() {
  try {
    auto units_mock = UnitsMap{{0, make_shared<Unit>(0, "X")}};
    auto mocked_variable = make_shared<Variable>(0,
        TextID("mock_var", "Mock variable"),
        AccessRights::ReadOnly,
        make_shared<BooleanT>());
    auto variables_mock = VariablesMap{{"mock", mocked_variable}};
    auto mocked_menu = make_shared<Menu>("Mocked UI",
        Menu::Refs{make_shared<VariableRef>(
            mocked_variable, VariableRef::ButtonValue(true))});
    auto mocked_ui = make_shared<UserInterface>(UserRole::ObservationRole,
        move(mocked_menu),
        nullptr,
        nullptr,
        nullptr);
    auto mocked_process_data = make_shared<ProcessDataUnion>("V_PdT",
        make_shared<ProcessDataT>("V_PdInT",
            64,
            TextID("TI_PdIn_Name", "Process Data Input/Output"),
            make_shared<UIntegerT>(64, NumberT<uint64_t>())));
    auto mocked_uis = DeviceDescriptor::UserInterfaces{
        {UserRole::ObservationRole, move(mocked_ui)}};

    auto device = make_shared<DeviceDescriptor>(0,
        "Hahn-Schickard",
        0,
        TextID("device_name", "Example Device"),
        make_shared<UnitsMap>(move(units_mock)),
        move(variables_mock),
        ProcessDataCollection{{"V_PdInT", move(mocked_process_data)}},
        move(mocked_uis));

    cout << device->getDeviceName().locale() << " has "
         << device->variableCount() << " variables" << endl;

    auto repo = makeRepository(filesystem::path("config"));
    cout << "IODD Repository has " << repo->size() << " descriptors" << endl;

    cout << "Integration test successful" << endl;
    exit(EXIT_SUCCESS);
  } catch (const exception& ex) {
    cerr << ex.what();
    exit(EXIT_FAILURE);
  }
}