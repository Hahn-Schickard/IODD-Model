#include "Serializer.hpp"

#include <pugixml.hpp>

#include <cstring>
#include <filesystem>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace pugi;

namespace IODD {

xml_document getXML(const filesystem::path& path) {
  if (!filesystem::exists(path)) {
    throw runtime_error(path.string() + " does not exists");
  }

  if (path.extension() != ".xml") {
    throw invalid_argument(path.string() + " is not an XML document");
  }

  xml_document xml;
  if (auto status = xml.load_file(path.c_str(), parse_default, encoding_utf8)) {
    return xml;
  } else {
    throw runtime_error("Failed to load " + path.string() +
        " as an XML document. " + status.description());
  }
}

TextID decodeLocalization(const xml_node& locales, const string& text_id) {
  string localization =
      locales.find_child_by_attribute("Text", "id", text_id.c_str())
          .attribute("value")
          .as_string();
  return TextID(text_id, localization);
}

Repository::UnitsMapPtr decodeUnits(const filesystem::path& path) {
  auto result = make_shared<Repository::UnitsMap>();
  auto xml = getXML(path).child("IODDStandardUnitDefinitions");
  auto units_collection = xml.child("UnitCollection");
  for (auto unit : units_collection.children("Unit")) {
    auto code = unit.attribute("code").as_uint();
    const auto* abbr = unit.attribute("abbr").as_string();
    const auto* text_id = unit.attribute("textId").as_string();
    result->emplace(
        code, make_shared<Unit>(code, abbr, decodeLocalization(xml, text_id)));
  }
  return result;
}

optional<AccessRights> decodeAccessRights(const xml_node& node) {
  string access_string = node.attribute("accessRights").as_string();
  if (!access_string.empty()) {
    if (access_string == "ro") {
      return AccessRights::READ_ONLY;
    } else if (access_string == "wo") {
      return AccessRights::WRITE_ONLY;
    } else if (access_string == "rw") {
      return AccessRights::READ_WRITE;
    }
  }
  return nullopt;
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
optional<TextID> decodeLocalizedText(
    const string& child_name, const xml_node& node, const xml_node& locales) {
  if (auto name_node = node.child(child_name.c_str()); !node.empty()) {
    return decodeLocalization(
        locales, name_node.attribute("textId").as_string());
  }
  return nullopt;
}

unordered_set<SingleValuePtr<bool>> decodeBoolSingleValues(
    const xml_node& node, const xml_node& locales) {
  unordered_set<SingleValuePtr<bool>> result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(
        make_shared<SingleValue<bool>>(node_value.attribute("value").as_bool(),
            decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<uint64_t>::SingleValues decodeUintSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<uint64_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<uint64_t>>(
        (uint64_t)node_value.attribute("value").as_ullong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<uint64_t>::ValueRanges decodeUintValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<uint64_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<uint64_t>>(
        (uint64_t)node_value.attribute("lowerValue").as_ullong(),
        (uint64_t)node_value.attribute("upperValue").as_ullong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<int64_t>::SingleValues decodeIntSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<int64_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<int64_t>>(
        (int64_t)node_value.attribute("value").as_llong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<int64_t>::ValueRanges decodeIntValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<int64_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<int64_t>>(
        (int64_t)node_value.attribute("lowerValue").as_llong(),
        (int64_t)node_value.attribute("upperValue").as_llong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<float>::SingleValues decodeFloatSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<float>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<float>>(
        node_value.attribute("value").as_float(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<float>::ValueRanges decodeFloatValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<float>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<float>>(
        node_value.attribute("lowerValue").as_float(),
        node_value.attribute("upperValue").as_float(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

template <typename T>
T decodeSimpleDataValue(
    const xml_node& /* node */, const xml_node& /* locales */) {
  throw runtime_error(
      "Failed to decode Simple Data Value. Unsupported data type");
}

template <>
BooleanT decodeSimpleDataValue(const xml_node& node, const xml_node& locales) {
  return BooleanT(decodeBoolSingleValues(node, locales));
}

template <>
UIntegerT decodeSimpleDataValue(const xml_node& node, const xml_node& locales) {
  auto length = node.attribute("bitLength").as_uint();
  return UIntegerT(length,
      decodeUintSingleValues(node, locales),
      decodeUintValueRanges(node, locales));
}

template <>
IntegerT decodeSimpleDataValue(const xml_node& node, const xml_node& locales) {
  auto length = node.attribute("bitLength").as_uint();
  return IntegerT(length,
      decodeIntSingleValues(node, locales),
      decodeIntValueRanges(node, locales));
}

template <>
FloatT decodeSimpleDataValue(const xml_node& node, const xml_node& locales) {
  return FloatT(decodeFloatSingleValues(node, locales),
      decodeFloatValueRanges(node, locales));
}

template <>
StringT decodeSimpleDataValue(
    const xml_node& node, const xml_node& /* locales */) {
  return StringT(node.attribute("fixedLength").as_uint(),
      (strcmp(node.attribute("encoding").as_string(), "UTF-8") != 0));
}

template <>
OctetStringT decodeSimpleDataValue(
    const xml_node& node, const xml_node& /* locales */) {
  return OctetStringT(node.attribute("fixedLength").as_uint());
}

template <>
TimeT decodeSimpleDataValue(
    const xml_node& /* node */, const xml_node& /* locales */) {
  return TimeT();
}

template <>
TimeSpanT decodeSimpleDataValue(
    const xml_node& /* node */, const xml_node& /* locales */) {
  return TimeSpanT();
}

SimpleDatatype decodeSimpleDataValue(
    Datatype type, const xml_node& node, const xml_node& locales) {
  switch (type) {
  case Datatype::Boolean: {
    return decodeSimpleDataValue<BooleanT>(node, locales);
  }
  case Datatype::UInteger: {
    return decodeSimpleDataValue<UIntegerT>(node, locales);
  }
  case Datatype::Integer: {
    return decodeSimpleDataValue<IntegerT>(node, locales);
  }
  case Datatype::Float32: {
    return decodeSimpleDataValue<FloatT>(node, locales);
  }
  case Datatype::String: {
    return decodeSimpleDataValue<StringT>(node, locales);
  }
  case Datatype::OctetString: {
    return decodeSimpleDataValue<OctetStringT>(node, locales);
  }
  case Datatype::Time: {
    return decodeSimpleDataValue<TimeT>(node, locales);
  }
  case Datatype::TimeSpan: {
    return decodeSimpleDataValue<TimeSpanT>(node, locales);
  default: {
    throw invalid_argument(toString(type) + " is not a simple data type");
  }
  }
  }
}

using ArrayValue = variant<ArrayT<BooleanT>,
    ArrayT<UIntegerT>,
    ArrayT<IntegerT>,
    ArrayT<FloatT>,
    ArrayT<OctetStringT>,
    ArrayT<StringT>,
    ArrayT<TimeT>,
    ArrayT<TimeSpanT>>;

template <typename T>
ArrayT<T> decodeArray(const Repository::DatatypesMap& datatypes_map,
    const xml_node& node,
    const xml_node& locale) {
  vector<T> values;

  if (!node.child("SimpleDatatype").empty()) {
    for (auto node_value : node.children("SimpleDatatype")) {
      values.push_back(decodeSimpleDataValue<T>(node_value, locale));
    }
  } else if (!node.child("DatatypeRef").empty()) {
    for (auto node_value : node.children("DatatypeRef")) {
      string id = node_value.attribute("datatypeId").as_string();
      if (auto it = datatypes_map.find(id); it != datatypes_map.end()) {
        values.push_back(get<T>(it->second));
      }
    }
  } else {
    throw runtime_error("Array has no specified data type");
  }

  return ArrayT(node.attribute("count").as_llong(),
      node.attribute("subindexAccessSupported").as_bool(true),
      move(values));
}

ArrayValue decodeArrayValue(const Repository::DatatypesMap& datatypes_map,
    const xml_node& node,
    const xml_node& locale) {
  auto type = toDatatype(
      node.child("SimpleDatatype").attribute("xsi:type").as_string());
  switch (type) {
  case Datatype::Boolean: {
    return decodeArray<BooleanT>(datatypes_map, node, locale);
  }
  case Datatype::UInteger: {
    return decodeArray<UIntegerT>(datatypes_map, node, locale);
  }
  case Datatype::Integer: {
    return decodeArray<IntegerT>(datatypes_map, node, locale);
  }
  case Datatype::Float32: {
    return decodeArray<FloatT>(datatypes_map, node, locale);
  }
  case Datatype::String: {
    return decodeArray<StringT>(datatypes_map, node, locale);
  }
  case Datatype::OctetString: {
    return decodeArray<OctetStringT>(datatypes_map, node, locale);
  }
  case Datatype::Time: {
    return decodeArray<TimeT>(datatypes_map, node, locale);
  }
  case Datatype::TimeSpan: {
    return decodeArray<TimeSpanT>(datatypes_map, node, locale);
  }
  default: {
    throw runtime_error("Failed to decode ArrayT. " + toString(type) +
        " is not a simple data type");
  }
  }
}

template <typename T>
RecordItem<T> decodeRecordItem(const Repository::DatatypesMap& datatypes_map,
    const xml_node& node,
    const xml_node& locales) {
  T value;

  if (auto node_value = node.child("SimpleDatatype"); !node_value.empty()) {
    value = decodeSimpleDataValue<T>(node_value, locales);
  } else if (auto node_value = node.child("DatatypeRef"); !node_value.empty()) {
    string id = node_value.attribute("datatypeId").as_string();
    if (auto it = datatypes_map.find(id); it != datatypes_map.end()) {
      value = get<T>(it->second);
    } else {
      throw runtime_error("Datatype Reference " + id +
          " not found in given datatypes collection");
    }
  } else {
    throw runtime_error("Record item has no specified data type");
  }

  auto subindex = node.attribute("subindex").as_ullong();
  auto offset = node.attribute("bitOffset").as_ullong();
  auto name = decodeLocalizedText("Name", node, locales).value();
  auto access = decodeAccessRights(node);
  auto desc = decodeLocalizedText("Description ", node, locales);

  return RecordItem(
      subindex, offset, move(value), move(name), access, move(desc));
}

template <typename T>
RecordT<T> decodeRecord(const Repository::DatatypesMap& datatypes_map,
    const xml_node& node,
    const xml_node& locales) {
  RecordItems<T> records;
  for (auto node_value : node.children("RecordItem")) {
    auto record = decodeRecordItem<T>(datatypes_map, node_value, locales);
    records.emplace(record.subindex(), move(record));
  }
  return RecordT<T>(node.attribute("bitLength").as_llong(),
      node.attribute("subindexAccessSupported").as_bool(true),
      move(records));
}

using RecordValue = variant<RecordT<BooleanT>,
    RecordT<UIntegerT>,
    RecordT<IntegerT>,
    RecordT<FloatT>,
    RecordT<OctetStringT>,
    RecordT<StringT>,
    RecordT<TimeT>,
    RecordT<TimeSpanT>>;

RecordValue decodeRecordValue(const Repository::DatatypesMap& datatypes_map,
    const xml_node& node,
    const xml_node& locales) {
  auto type = toDatatype(
      node.child("SimpleDatatype").attribute("xsi:type").as_string());
  switch (type) {
  case Datatype::Boolean: {
    return decodeRecord<BooleanT>(datatypes_map, node, locales);
  }
  case Datatype::UInteger: {
    return decodeRecord<UIntegerT>(datatypes_map, node, locales);
  }
  case Datatype::Integer: {
    return decodeRecord<IntegerT>(datatypes_map, node, locales);
  }
  case Datatype::Float32: {
    return decodeRecord<FloatT>(datatypes_map, node, locales);
  }
  case Datatype::String: {
    return decodeRecord<StringT>(datatypes_map, node, locales);
  }
  case Datatype::OctetString: {
    return decodeRecord<OctetStringT>(datatypes_map, node, locales);
  }
  case Datatype::Time: {
    return decodeRecord<TimeT>(datatypes_map, node, locales);
  }
  case Datatype::TimeSpan: {
    return decodeRecord<TimeSpanT>(datatypes_map, node, locales);
  }
  default: {
    throw runtime_error("Failed to decode RecordT. " + toString(type) +
        " is not a simple data type");
  }
  }
}

template <class... Args> struct VariantCaster {
  std::variant<Args...> v;

  template <class... ToArgs> operator std::variant<ToArgs...>() const {
    return std::visit(
        [](auto&& arg) -> std::variant<ToArgs...> { return arg; }, v);
  }
};

template <class... Args>
auto variantCast(const std::variant<Args...>& v) -> VariantCaster<Args...> {
  return {v};
}

DataValue decodeDataValue(const xml_node& node,
    const xml_node& locales,
    Datatype type,
    const Repository::DatatypesMap& datatypes_map = {}) {
  switch (type) {
  case Datatype::Array: {
    return variantCast(decodeArrayValue(datatypes_map, node, locales));
  }
  case Datatype::Record: {
    return variantCast(decodeRecordValue(datatypes_map, node, locales));
  }
  case Datatype::ProcessDataIn: {
    throw logic_error("Failed to decoded ProcessDataIn DataType Values");
  }
  case Datatype::ProcessDataOut: {
    throw logic_error("Failed to decoded ProcessDataOut DataType Values");
  }
  default: {
    return variantCast(decodeSimpleDataValue(type, node, locales));
  }
  }
}
// NOLINTEND(bugprone-easily-swappable-parameters)

Repository::DatatypesMap decodeDatatypes(const xml_node& xml,
    const xml_node& locales,
    const Repository::DatatypesMap& std_datatypes = {}) {
  Repository::DatatypesMap datatypes = std_datatypes;
  for (auto datatype : xml.children("Datatype")) {
    datatypes.emplace(datatype.attribute("id").as_string(),
        decodeDataValue(datatype,
            locales,
            toDatatype(datatype.attribute("xsi:type").as_string())));
  }
  return datatypes;
}

Repository::VariablesMap decodeVariables(const xml_node& xml,
    const xml_node& locales,
    const Repository::DatatypesMap& datatypes,
    const Repository::VariablesMap& std_variables = {}) {
  Repository::VariablesMap variables = std_variables;
  for (auto variable : xml.children("Variable")) {
    variables.emplace(variable.attribute("id").as_string(),
        make_shared<Variable>(variable.attribute("index").as_ullong(),
            decodeLocalizedText("Name", variable, locales).value(),
            decodeAccessRights(variable).value(),
            decodeDataValue(variable,
                locales,
                toDatatype(variable.attribute("xsi:type").as_string()),
                datatypes),
            decodeLocalizedText("Description", variable, locales).value(),
            nullopt,
            variable.attribute("dynamic").as_bool(false),
            variable.attribute("modifiesOtherVariables").as_bool(false),
            variable.attribute("excludedFromDataStorage").as_bool(false)));
  }
  return variables;
}

pair<Repository::DatatypesMapPtr, Repository::VariablesMapPtr>
decodeStdDefinitions(const filesystem::path& path) {
  auto xml = getXML(path).child("IODDStandardUnitDefinitions");
  auto locales_xml =
      xml.child("ExternalTextCollection").child("PrimaryLanguage");

  auto datatypes =
      decodeDatatypes(xml.child("DatatypeCollection"), locales_xml);
  auto variables =
      decodeVariables(xml.child("VariableCollection"), locales_xml, datatypes);

  return make_pair(make_shared<Repository::DatatypesMap>(datatypes),
      make_shared<Repository::VariablesMap>(variables));
}

DeviceIdentity decodeIdentity(const xml_node& node, const xml_node& locales) {
  if (auto identity_xml = node.child("DeviceIdentity"); !identity_xml.empty()) {
    auto vendor_id = identity_xml.attribute("vendorId").as_uint();
    string vendor_name = identity_xml.attribute("vendorName").as_string();
    auto device_id = identity_xml.attribute("vendorId").as_uint();
    auto device_name = decodeLocalization(
        locales, node.child("DeviceName").attribute("textId").as_string());
    return DeviceIdentity(vendor_id, vendor_name, device_id, device_name);
  } else {
    throw runtime_error(
        "DeviceIdentity node not found in a given XML structure");
  }
}

Repository::VariablesMap operator+=(
    Repository::VariablesMap lhs, const Repository::VariablesMap& rhs) {
  lhs.insert(rhs.begin(), rhs.end());
  return lhs;
}

SimpleDatatypeValue decodeDefaultValue(
    Datatype type, const xml_attribute& attribute) {
  switch (type) {
  case Datatype::Boolean: {
    return attribute.as_bool();
  }
  case Datatype::UInteger: {
    return attribute.as_ullong();
  }
  case Datatype::Integer: {
    return attribute.as_llong();
  }
  case Datatype::Float32: {
    return attribute.as_float();
  }
  case Datatype::String: {
    [[fallthrough]];
  }
  case Datatype::Time: {
    [[fallthrough]];
  }
  case Datatype::TimeSpan: {
    [[fallthrough]];
  }
  case Datatype::OctetString: {
    return attribute.as_string();
  }
  default: {
    throw invalid_argument(toString(type) + " is not a simple data type");
  }
  }
}

optional<DataValue> getUpdatedValues(IODD::Datatype type,
    const Repository::DatatypesMapPtr& datatypes,
    const xml_node& node,
    const xml_node& locales) {
  try {
    auto simple_value = decodeSimpleDataValue(type, node, locales);
    return variantCast(simple_value);
  } catch (const invalid_argument& ex) {
    // TODO: decode StdRecordItemRef
  }
  return nullopt;
}

Repository::VariablesMap decodeStdVariableRef(const xml_node& xml,
    const xml_node& locales,
    const Repository::DatatypesMapPtr& datatypes,
    const Repository::VariablesMapPtr& std_variables) {
  Repository::VariablesMap variables = *std_variables;
  for (auto variable : xml.children("StdVariableRef")) {
    string id = variable.attribute("id").as_string();
    // ignore parameter overlays
    if (id != "V_DirectParameters_1" || id != "V_DirectParameters_2") {
      if (auto std_variable = variables.find(id);
          std_variable != variables.end()) {
        optional<SimpleDatatypeValue> default_value = nullopt;
        if (auto node_value = variable.attribute("defaultValue");
            !node_value.empty()) {
          default_value =
              decodeDefaultValue(std_variable->second->type(), node_value);
        }
        optional<bool> excluded = nullopt;
        if (auto historized = variable.attribute("excludedFromDataStorage");
            !historized.empty()) {
          excluded = historized.as_bool();
        }
        auto possible_value = getUpdatedValues(
            std_variable->second->type(), datatypes, variable, locales);
        // check if variable needs to be updated
        if (default_value.has_value() || excluded.has_value() ||
            possible_value.has_value()) {
          std_variable->second = make_shared<Variable>(
              *std_variable->second, default_value, excluded, possible_value);
        }
      } else {
        throw runtime_error(id + " is not defined in standard variables");
      }
    }
  }
  return variables;
}

DeviceDescriptorPtr decode(const Repository::UnitsMapPtr& units,
    const Repository::DatatypesMapPtr& std_datatypes,
    const Repository::VariablesMapPtr& std_variables,
    const xml_document& xml) {
  auto device_xml = xml.child("IODevice");
  auto locales_xml =
      device_xml.child("ExternalTextCollection").child("PrimaryLanguage");
  auto profile_xml = device_xml.child("ProfileBody");
  auto identity = decodeIdentity(profile_xml, locales_xml);
  auto function_xml = device_xml.child("DeviceFunction");
  auto variables_xml = function_xml.child("VariableCollection");
  auto variables = decodeStdVariableRef(
      variables_xml, locales_xml, std_datatypes, std_variables);
  variables += decodeVariables(variables_xml, locales_xml, *std_datatypes);

  return make_shared<DeviceDescriptor>(
      move(identity), units, std_variables, move(variables));
}

Repository::DescriptorsMap decodeDescriptors(
    const Repository::UnitsMapPtr& units,
    const pair<Repository::DatatypesMapPtr, Repository::VariablesMapPtr>&
        variables,
    const filesystem::path& path) {
  Repository::DescriptorsMap descriptors;

  for (const auto& entry : filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".xml") {
      xml_document descriptor_xml = getXML(entry.path());
      auto descriptor =
          decode(units, variables.first, variables.second, descriptor_xml);
      descriptors.emplace(descriptor->getIdentifier(), move(descriptor));
    }
  }
  return descriptors;
}

Repository deserializeModel(const string& config_directory_path) {
  auto config_dir = filesystem::path(config_directory_path);

  if (!filesystem::is_directory(config_dir)) {
    throw invalid_argument(config_dir.string() + " is not a directory");
  }

  auto std_units_map =
      decodeUnits(config_dir / "IODD-StandardUnitDefinitions1.1.1.xml");
  auto std_variables_map =
      decodeStdDefinitions(config_dir / "IODD-StandardDefinitions1.1.xml");
  auto descriptors = decodeDescriptors(
      std_units_map, std_variables_map, config_dir / "descriptors");
  return Repository(
      move(std_units_map), move(std_variables_map), move(descriptors));
}
} // namespace IODD