#include "Serializer.hpp"

#include <pugixml.hpp>

#include <cstring>
#include <filesystem>
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

TextID decodeLocalization(const xml_node& node, const string& text_id) {
  auto localizations =
      node.child("ExternalTextCollection").child("PrimaryLanguage");
  const auto* localization =
      localizations.find_child_by_attribute("Text", "id", text_id.c_str())
          .attribute("value")
          .as_string();
  return TextID(text_id, localization);
}

Repository::UnitsMap decodeUnits(const filesystem::path& path) {
  Repository::UnitsMap result;
  auto xml = getXML(path).child("IODDStandardUnitDefinitions");
  auto units_collection = xml.child("UnitCollection");
  for (auto unit : units_collection.children("Unit")) {
    auto code = unit.attribute("code").as_uint();
    const auto* abbr = unit.attribute("abbr").as_string();
    const auto* text_id = unit.attribute("textId").as_string();
    result.emplace(code, Unit(code, abbr, decodeLocalization(xml, text_id)));
  }
  return result;
}

enum class Datatype {
  Boolean,
  UInteger,
  Integer,
  Float32,
  String,
  OctetString,
  Time,
  TimeSpan,
  Array,
  Record,
  ProcessDataIn,
  ProcessDataOut
};

string toString(Datatype type) {
  switch (type) {
  case Datatype::Boolean: {
    return "Boolean";
  }
  case Datatype::UInteger: {
    return "UInteger";
  }
  case Datatype::Integer: {
    return "Integer";
  }
  case Datatype::Float32: {
    return "Float32";
  }
  case Datatype::String: {
    return "String";
  }
  case Datatype::OctetString: {
    return "OctetString";
  }
  case Datatype::Time: {
    return "Time";
  }
  case Datatype::TimeSpan: {
    return "TimeSpan";
  }
  case Datatype::Array: {
    return "Array";
  }
  case Datatype::Record: {
    return "Record";
  }
  case Datatype::ProcessDataIn: {
    return "ProcessDataIn";
  }
  case Datatype::ProcessDataOut: {
    return "ProcessDataOut";
  }
  default: {
    throw runtime_error("Unhandled Datatype enum value");
  }
  }
}

Datatype toDatatype(const string& value) {
  const unordered_map<string, Datatype> data_types = {
      {"BooleanT", Datatype::Boolean}, {"UIntegerT", Datatype::UInteger},
      {"IntegerT", Datatype::Integer}, {"Float32T", Datatype::Float32},
      {"StringT", Datatype::String}, {"OctetStringT", Datatype::OctetString},
      {"TimeT", Datatype::Time}, {"TimeSpanT", Datatype::TimeSpan},
      {"ArrayT", Datatype::Array}, {"RecordT", Datatype::Record},
      {"ProcessDataInUnionT", Datatype::ProcessDataIn},
      {"ProcessDataOutUnionT", Datatype::ProcessDataOut}};

  if (auto it = data_types.find(value); it != data_types.end()) {
    return it->second;
  }
  throw invalid_argument("String value: " + value +
      " can not be converted into IODD::Datatype enumeration");
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
    const string& child_name, const xml_node& root, const xml_node& node) {
  if (auto name_node = node.child(child_name.c_str()); !node.empty()) {
    return decodeLocalization(root, name_node.attribute("textId").as_string());
  }
  return nullopt;
}

unordered_set<SingleValuePtr<bool>> decodeBoolSingleValues(
    const xml_node& root, const xml_node& node) {
  unordered_set<SingleValuePtr<bool>> result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(
        make_shared<SingleValue<bool>>(node_value.attribute("value").as_bool(),
            decodeLocalizedText("Name", root, node_value)));
  }
  return result;
}

NumberT<size_t>::SingleValues decodeUintSingleValues(
    const xml_node& root, const xml_node& node) {
  NumberT<size_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<size_t>>(
        node_value.attribute("value").as_ullong(),
        decodeLocalizedText("Name", root, node_value)));
  }
  return result;
}

NumberT<size_t>::ValueRanges decodeUintValueRanges(
    const xml_node& root, const xml_node& node) {
  NumberT<size_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<size_t>>(
        node_value.attribute("lowerValue").as_ullong(),
        node_value.attribute("upperValue").as_ullong(),
        decodeLocalizedText("Name", root, node_value)));
  }
  return result;
}

NumberT<intmax_t>::SingleValues decodeIntSingleValues(
    const xml_node& root, const xml_node& node) {
  NumberT<intmax_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<intmax_t>>(
        node_value.attribute("value").as_llong(),
        decodeLocalizedText("Name", root, node_value)));
  }
  return result;
}

NumberT<intmax_t>::ValueRanges decodeIntValueRanges(
    const xml_node& root, const xml_node& node) {
  NumberT<intmax_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<intmax_t>>(
        node_value.attribute("lowerValue").as_llong(),
        node_value.attribute("upperValue").as_llong(),
        decodeLocalizedText("Name", root, node_value)));
  }
  return result;
}

NumberT<float>::SingleValues decodeFloatSingleValues(
    const xml_node& root, const xml_node& node) {
  NumberT<float>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<float>>(
        node_value.attribute("value").as_float(),
        decodeLocalizedText("Name", root, node_value)));
  }
  return result;
}

NumberT<float>::ValueRanges decodeFloatValueRanges(
    const xml_node& root, const xml_node& node) {
  NumberT<float>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<float>>(
        node_value.attribute("lowerValue").as_float(),
        node_value.attribute("upperValue").as_float(),
        decodeLocalizedText("Name", root, node_value)));
  }
  return result;
}

template <typename T>
T decodeSimpleDataValue(
    const xml_node& /* root */, const xml_node& /* node */) {
  throw runtime_error(
      "Failed to decode Simple Data Value. Unsupported data type");
}

template <>
BooleanT decodeSimpleDataValue(const xml_node& root, const xml_node& node) {
  return BooleanT(decodeBoolSingleValues(root, node));
}

template <>
UIntegerT decodeSimpleDataValue(const xml_node& root, const xml_node& node) {
  auto length = node.attribute("bitLength").as_uint();
  return UIntegerT(length, decodeUintSingleValues(root, node),
      decodeUintValueRanges(root, node));
}

template <>
IntegerT decodeSimpleDataValue(const xml_node& root, const xml_node& node) {
  auto length = node.attribute("bitLength").as_uint();
  return IntegerT(length, decodeIntSingleValues(root, node),
      decodeIntValueRanges(root, node));
}

template <>
FloatT decodeSimpleDataValue(const xml_node& root, const xml_node& node) {
  return FloatT(
      decodeFloatSingleValues(root, node), decodeFloatValueRanges(root, node));
}

template <>
StringT decodeSimpleDataValue(
    const xml_node& /* root */, const xml_node& node) {
  return StringT(node.attribute("fixedLength").as_uint(),
      (strcmp(node.attribute("encoding").as_string(), "UTF-8") != 0));
}

template <>
OctetStringT decodeSimpleDataValue(
    const xml_node& /* root */, const xml_node& node) {
  return OctetStringT(node.attribute("fixedLength").as_uint());
}

template <>
TimeT decodeSimpleDataValue(
    const xml_node& /* root */, const xml_node& /* node */) {
  return TimeT();
}

template <>
TimeSpanT decodeSimpleDataValue(
    const xml_node& /* root */, const xml_node& /* node */) {
  return TimeSpanT();
}

using SimpleDatatype = std::variant<BooleanT, UIntegerT, IntegerT, FloatT,
    OctetStringT, StringT, TimeT, TimeSpanT>;

SimpleDatatype decodeSimpleDataValue(
    const xml_node& root, const xml_node& node, Datatype type) {
  switch (type) {
  case Datatype::Boolean: {
    return decodeSimpleDataValue<BooleanT>(root, node);
  }
  case Datatype::UInteger: {
    return decodeSimpleDataValue<UIntegerT>(root, node);
  }
  case Datatype::Integer: {
    return decodeSimpleDataValue<IntegerT>(root, node);
  }
  case Datatype::Float32: {
    return decodeSimpleDataValue<FloatT>(root, node);
  }
  case Datatype::String: {
    return decodeSimpleDataValue<StringT>(root, node);
  }
  case Datatype::OctetString: {
    return decodeSimpleDataValue<OctetStringT>(root, node);
  }
  case Datatype::Time: {
    return decodeSimpleDataValue<TimeT>(root, node);
  }
  case Datatype::TimeSpan: {
    return decodeSimpleDataValue<TimeSpanT>(root, node);
  default: {
    throw invalid_argument(toString(type) + " is not a simple data type");
  }
  }
  }
}

using ArrayValue = variant<ArrayT<BooleanT>, ArrayT<UIntegerT>,
    ArrayT<IntegerT>, ArrayT<FloatT>, ArrayT<OctetStringT>, ArrayT<StringT>,
    ArrayT<TimeT>, ArrayT<TimeSpanT>>;

template <typename T>
ArrayT<T> decodeArray(const xml_node& root, const xml_node& node,
    const Repository::DatatypesMap& datatypes_map) {
  vector<T> values;

  if (node.child("SimpleDatatype").hash_value()) {
    for (auto node_value : node.children("SimpleDatatype")) {
      values.push_back(decodeSimpleDataValue<T>(root, node_value));
    }
  } else if (node.child("DatatypeRef").hash_value()) {
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
      node.attribute("subindexAccessSupported").as_bool(true), move(values));
}

ArrayValue decodeArrayValue(const xml_node& root, const xml_node& node,
    const Repository::DatatypesMap& datatypes_map) {
  auto type = toDatatype(
      node.child("SimpleDatatype").attribute("xsi:type").as_string());
  switch (type) {
  case Datatype::Boolean: {
    return decodeArray<BooleanT>(root, node, datatypes_map);
  }
  case Datatype::UInteger: {
    return decodeArray<UIntegerT>(root, node, datatypes_map);
  }
  case Datatype::Integer: {
    return decodeArray<IntegerT>(root, node, datatypes_map);
  }
  case Datatype::Float32: {
    return decodeArray<FloatT>(root, node, datatypes_map);
  }
  case Datatype::String: {
    return decodeArray<StringT>(root, node, datatypes_map);
  }
  case Datatype::OctetString: {
    return decodeArray<OctetStringT>(root, node, datatypes_map);
  }
  case Datatype::Time: {
    return decodeArray<TimeT>(root, node, datatypes_map);
  }
  case Datatype::TimeSpan: {
    return decodeArray<TimeSpanT>(root, node, datatypes_map);
  }
  default: {
    throw runtime_error("Failed to decode ArrayT. " + toString(type) +
        " is not a simple data type");
  }
  }
}

template <typename T>
RecordItem<T> decodeRecordItem(const xml_node& root, const xml_node& node,
    const Repository::DatatypesMap& datatypes_map) {
  T value;

  if (auto node_value = node.child("SimpleDatatype"); node_value.hash_value()) {
    value = decodeSimpleDataValue<T>(root, node_value);
  } else if (auto node_value = node.child("DatatypeRef");
             node_value.hash_value()) {
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
  auto name = decodeLocalizedText("Name", root, node).value();
  auto access = decodeAccessRights(node);
  auto desc = decodeLocalizedText("Description ", root, node);

  return RecordItem(
      subindex, offset, move(value), move(name), access, move(desc));
}

template <typename T>
RecordT<T> decodeRecord(const xml_node& root, const xml_node& node,
    const Repository::DatatypesMap& datatypes_map) {
  RecordItems<T> records;
  for (auto node_value : node.children("RecordItem")) {
    records.emplace(decodeRecordItem<T>(root, node_value, datatypes_map));
  }
  return RecordT<T>(node.attribute("bitLength").as_llong(),
      node.attribute("subindexAccessSupported").as_bool(true), move(records));
}

using RecordValue = variant<RecordT<BooleanT>, RecordT<UIntegerT>,
    RecordT<IntegerT>, RecordT<FloatT>, RecordT<OctetStringT>, RecordT<StringT>,
    RecordT<TimeT>, RecordT<TimeSpanT>>;

RecordValue decodeRecordValue(const xml_node& root, const xml_node& node,
    const Repository::DatatypesMap& datatypes_map) {
  auto type = toDatatype(
      node.child("SimpleDatatype").attribute("xsi:type").as_string());
  switch (type) {
  case Datatype::Boolean: {
    return decodeRecord<BooleanT>(root, node, datatypes_map);
  }
  case Datatype::UInteger: {
    return decodeRecord<UIntegerT>(root, node, datatypes_map);
  }
  case Datatype::Integer: {
    return decodeRecord<IntegerT>(root, node, datatypes_map);
  }
  case Datatype::Float32: {
    return decodeRecord<FloatT>(root, node, datatypes_map);
  }
  case Datatype::String: {
    return decodeRecord<StringT>(root, node, datatypes_map);
  }
  case Datatype::OctetString: {
    return decodeRecord<OctetStringT>(root, node, datatypes_map);
  }
  case Datatype::Time: {
    return decodeRecord<TimeT>(root, node, datatypes_map);
  }
  case Datatype::TimeSpan: {
    return decodeRecord<TimeSpanT>(root, node, datatypes_map);
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

DataValue decodeDataValue(const xml_node& root, const xml_node& node,
    Datatype type, const Repository::DatatypesMap& datatypes_map = {}) {
  switch (type) {
  case Datatype::Array: {
    return variantCast(decodeArrayValue(root, node, datatypes_map));
  }
  case Datatype::Record: {
    return variantCast(decodeRecordValue(root, node, datatypes_map));
  }
  case Datatype::ProcessDataIn: {
    throw logic_error("Failed to decoded ProcessDataIn DataType Values");
  }
  case Datatype::ProcessDataOut: {
    throw logic_error("Failed to decoded ProcessDataOut DataType Values");
  }
  default: {
    return variantCast(decodeSimpleDataValue(root, node, type));
  }
  }
}
// NOLINTEND(bugprone-easily-swappable-parameters)

pair<Repository::DatatypesMap, Repository::VariablesMap> decodeStdDefinitions(
    const filesystem::path& path) {
  auto xml = getXML(path).child("IODDStandardUnitDefinitions");
  auto datatype_collection = xml.child("DatatypeCollection");
  Repository::DatatypesMap datatypes;
  for (auto datatype : datatype_collection.children("Datatype")) {
    datatypes.emplace(datatype.attribute("id").as_string(),
        decodeDataValue(xml, datatype,
            toDatatype(datatype.attribute("xsi:type").as_string())));
  }

  auto variable_collection = xml.child("VariableCollection");
  Repository::VariablesMap variables;
  for (auto variable : variable_collection.children("Variable")) {
    variables.emplace(variable.attribute("id").as_string(),
        Variable(variable.attribute("index").as_ullong(),
            decodeLocalizedText("Name", xml, variable).value(),
            decodeAccessRights(variable).value(),
            decodeDataValue(xml, variable,
                toDatatype(variable.attribute("xsi:type").as_string()),
                datatypes),
            decodeLocalizedText("Description", xml, variable).value(),
            variable.attribute("dynamic").as_bool(false),
            variable.attribute("modifiesOtherVariables").as_bool(false),
            variable.attribute("excludedFromDataStorage").as_bool(false)));
  }

  return make_pair(move(datatypes), move(variables));
}

DeviceDescriptorPtr decode(const Repository::UnitsMap& units,
    const Repository::DatatypesMap& datatypes,
    const Repository::VariablesMap& variables, const xml_document& xml) {
  auto device_xml = xml.child("IODevice");
  auto locales_xml = device_xml.child("ExternalTextCollection");
  auto profile_xml = device_xml.child("ProfileBody");
}

Repository::DescriptorsMap decodeDescriptors(const Repository::UnitsMap& units,
    const pair<Repository::DatatypesMap, Repository::VariablesMap>& variables,
    const filesystem::path& path) {
  Repository::DescriptorsMap descriptors;

  for (const auto& entry : filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".xml") {
      xml_document descriptor_xml = getXML(entry.path());
      auto descriptor = decode(descriptor_xml);
      descriptors.emplace(descriptor->getDeviceIdentity(), descriptor);
    }
  }
  return descriptors;
}

Repository deserializeModel(const string& config_directory_path) {
  auto config_dir = filesystem::path(config_directory_path);

  if (!filesystem::is_directory(config_dir)) {
    throw invalid_argument(config_dir.string() + " is not a directory");
  }

  auto std_units_filepath = config_dir / "IODD-StandardDefinitions1.1.xml";
  auto std_variables_filepath =
      config_dir / "IODD-StandardUnitDefinitions1.1.1.xml";
  auto descriptors_dir = config_dir / "descriptors";
  return Repository(decodeUnits(std_units_filepath),
      decodeStdDefinitions(std_variables_filepath),
      decodeDescriptors(descriptors_dir));
}
} // namespace IODD