#include "Serializer.hpp"

#include "Decoders/DataValueDecoder.hpp"
#include "Decoders/PrimitivesDecoder.hpp"
#include "Decoders/UnitDecoder.hpp"
#include "Decoders/UserInterfaceDecoder.hpp"
#include "Decoders/VariablesDecoder.hpp"
#include "Decoders/XML_Helper.hpp"

#include <cstring>
#include <filesystem>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace pugi;

namespace IODD {
pair<DatatypesMapPtr, VariablesMapPtr> decodeStdDefinitions(
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

    return make_pair(make_shared<DatatypesMap>(datatypes),
        make_shared<VariablesMap>(variables));
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

DeviceDescriptorPtr decode(const UnitsMapPtr& units,
    const DatatypesMapPtr& std_datatypes,
    const VariablesMapPtr& std_variables,
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
    auto variables_map = make_shared<VariablesMap>(variables);

    auto ui_xml = getXMLNode("UserInterface", function_xml);
    auto uis =
        decodeUI(units, variables_map, std_variables, ui_xml, locales_xml);

    return make_shared<DeviceDescriptor>(
        move(identity), units, std_variables, move(variables_map), move(uis));
  } catch (const exception& ex) {
    throw runtime_error("Failed to decode file " + doc.string() +
        " due to exception: " + ex.what());
  }
}

Repository::DescriptorsMap decodeDescriptors(const UnitsMapPtr& units,
    const pair<DatatypesMapPtr, VariablesMapPtr>& variables,
    const filesystem::path& path) {
  Repository::DescriptorsMap descriptors;

  for (const auto& entry : filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".xml") {
      auto descriptor =
          decode(units, variables.first, variables.second, entry.path());
      descriptors.emplace(descriptor->getIdentifier(), move(descriptor));
    }
  }
  return descriptors;
}

Repository deserializeModel(const std::filesystem::path& dir) {
  if (!filesystem::is_directory(dir)) {
    throw invalid_argument(dir.string() + " is not a directory");
  }

  auto std_units_map = decodeUnits(dir / "IODD-StandardUnitDefinitions.xml");
  auto std_variables_map =
      decodeStdDefinitions(dir / "IODD-StandardDefinitions.xml");
  auto descriptors =
      decodeDescriptors(std_units_map, std_variables_map, dir / "descriptors");
  return Repository(
      move(std_units_map), move(std_variables_map), move(descriptors));
}
} // namespace IODD