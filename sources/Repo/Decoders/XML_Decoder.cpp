#include "XML_Decoder.hpp"

#include "DataValueDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "ProcessDataDecoder.hpp"
#include "UnitDecoder.hpp"
#include "UserInterfaceDecoder.hpp"
#include "VariablesDecoder.hpp"
#include "XML_Helper.hpp"

#include <stdexcept>

using namespace std;
using namespace pugi;

namespace IODD {
pair<DatatypesMapPtr, VariablesMap> decodeStdDefinitions(
    const filesystem::path& path) {
  try {
    auto doc = getXML(path);

    auto xml = getXMLNode("IODDStandardDefinitions", doc, path);
    auto locales_xml = getXMLNode(
        vector<string>{"ExternalTextCollection", "PrimaryLanguage"}, xml, path);

    auto datatype_collection_xml = getXMLNode("DatatypeCollection", xml, path);
    auto datatypes = decodeDatatypes(datatype_collection_xml, locales_xml);

    auto variable_collection_xml = getXMLNode("VariableCollection", xml, path);
    auto variables =
        decodeVariables(variable_collection_xml, locales_xml, datatypes);

    return make_pair(make_shared<DatatypesMap>(datatypes), variables);
  } catch (const exception& ex) {
    throw runtime_error("Caught an exception while processing " +
        path.string() + " Exception: " + ex.what());
  }
}

DeviceIdentity decodeIdentity(const xml_node& node, const xml_node& locales) {
  if (auto identity_xml = node.child("DeviceIdentity"); !identity_xml.empty()) {
    auto vendor_id = getXMLAttribute("vendorId", identity_xml).as_uint();
    string vendor_name =
        getXMLAttribute("vendorName", identity_xml).as_string();
    auto device_id = getXMLAttribute("deviceId", identity_xml).as_uint();
    string device_name_attribute =
        getXMLAttribute("textId", getXMLNode("DeviceName", identity_xml))
            .as_string();
    auto device_name = decodeLocalization(locales, device_name_attribute);
    return DeviceIdentity(vendor_id, vendor_name, device_id, device_name);
  } else {
    throw runtime_error(
        "DeviceIdentity node not found in a given XML structure");
  }
}

void updateVariableValue(
    const string& name, VariablesMap& variables, const ProcessDataTPtr& data) {
  auto var = findVariable(name, variables);
  variables[name] = make_shared<Variable>(*var, data);
}

void setVariableProcessData(VariablesMap& variables,
    const ProcessDataCollection& process_data_collection) {
  if (process_data_collection.size() > 1) {
    throw runtime_error("Setting multiple ProcessData values for variables via "
                        "conditions is not supported");
  }
  auto process_data_it = process_data_collection.begin();
  if (process_data_it != process_data_collection.end()) {
    auto process_data = process_data_it->second;
    if (process_data->inData()) {
      updateVariableValue(
          "V_ProcessDataInput", variables, process_data->inData());
    }
    if (process_data->outData()) {
      updateVariableValue(
          "V_ProcessDataOutput", variables, process_data->outData());
    }
  } else {
    throw runtime_error("ProcessDataCollection is empty");
  }
}

DeviceDescriptorPtr decode(const UnitsMapPtr& units,
    const DatatypesMapPtr& std_datatypes,
    const VariablesMap& std_variables,
    const filesystem::path& doc) {
  try {
    auto xml = getXML(doc);

    auto device_xml = getXMLNode("IODevice", xml);
    auto locales_xml =
        getXMLNode(vector<string>{"ExternalTextCollection", "PrimaryLanguage"},
            device_xml);

    auto profile_xml = getXMLNode("ProfileBody", device_xml);
    auto identity = decodeIdentity(profile_xml, locales_xml);

    auto function_xml = getXMLNode("DeviceFunction", profile_xml);

    auto variables_xml = getXMLNode("VariableCollection", function_xml);
    auto variables = decodeStdVariables(
        variables_xml, locales_xml, std_datatypes, std_variables);
    for (const auto& variable :
        decodeVariables(variables_xml, locales_xml, *std_datatypes)) {
      variables.insert(variable);
    }

    auto process_xml = getXMLNode("ProcessDataCollection", function_xml);
    auto process_data =
        decodeProcessData(process_xml, locales_xml, variables, std_datatypes);
    setVariableProcessData(variables, process_data);

    auto ui_xml = getXMLNode("UserInterface", function_xml);
    auto uis = decodeUI(units, variables, ui_xml, locales_xml);

    return make_shared<DeviceDescriptor>(
        move(identity), units, move(variables), move(process_data), move(uis));
  } catch (const exception& ex) {
    throw runtime_error("Failed to decode file " + doc.string() +
        " due to exception: " + ex.what());
  }
}

DescriptorsMap decodeDescriptors(const UnitsMapPtr& units,
    const pair<DatatypesMapPtr, VariablesMap>& variables,
    const filesystem::path& path) {
  DescriptorsMap descriptors;

  for (const auto& entry : filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".xml") {
      auto descriptor =
          decode(units, variables.first, variables.second, entry.path());
      descriptors.emplace(descriptor->getIdentifier(), move(descriptor));
    }
  }
  return descriptors;
}
} // namespace IODD