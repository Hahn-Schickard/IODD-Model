#include "Serializer.hpp"

#include <pugixml.hpp>

#include <cstring>
#include <filesystem>
#include <stdexcept>

using namespace std;
using namespace pugi;

namespace IODD {

IODeviceDescriptorPtr decode(const xml_document& xml) {}

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

SimpleDatatype decodeSimpleDataValue(
    const xml_node& root, const xml_node& node, Datatype type) {
  switch (type) {
  case Datatype::Boolean: {
    return BooleanT(decodeBoolSingleValues(root, node));
  }
  case Datatype::UInteger: {
    auto length = node.attribute("bitLength").as_uint();
    return UIntegerT(length, decodeUintSingleValues(root, node),
        decodeUintValueRanges(root, node));
  }
  case Datatype::Integer: {
    auto length = node.attribute("bitLength").as_uint();
    return IntegerT(length, decodeIntSingleValues(root, node),
        decodeIntValueRanges(root, node));
  }
  case Datatype::Float32: {
    return FloatT(decodeFloatSingleValues(root, node),
        decodeFloatValueRanges(root, node));
  }
  case Datatype::String: {
    return StringT(node.attribute("fixedLength").as_uint(),
        strcmp(node.attribute("encoding").as_string(), "UTF-8"));
  }
  case Datatype::OctetString: {
    return OctetStringT(node.attribute("fixedLength").as_uint());
  }
  case Datatype::Time: {
    return TimeT();
  }
  case Datatype::TimeSpan: {
    return TimeSpanT();
  }
  default: {
    throw invalid_argument(toString(type) + " is not a simple data type");
  }
  }
}

ArrayT decodeArray(const xml_node& root, const xml_node& node) {
  vector<SimpleDatatype> values;
  for (auto node_value : node.children("SimpleDatatype")) {
    values.push_back(decodeSimpleDataValue(root, node_value,
        toDatatype(node_value.attribute("xsi:type").as_string())));
  }
  // @TODO: obtain DatatypeRef as well and insert it into values
  return ArrayT(node.attribute("count").as_llong(),
      node.attribute("subindexAccessSupported").as_bool(true), move(values));
}

RecordItem decodeRecordItem(const xml_node& root, const xml_node& node) {
  auto subindex = node.attribute("subindex").as_ullong();
  auto offset = node.attribute("bitOffset").as_ullong();
  auto node_value = node.child("SimpleDatatype");
  auto type = decodeSimpleDataValue(root, node_value,
      toDatatype(node_value.attribute("xsi:type").as_string()));
  // @TODO: obtain DatatypeRef as well and insert it into type
  auto name = decodeLocalizedText("Name", root, node).value();
  auto access = decodeAccessRights(node);
  auto desc = decodeLocalizedText("Description ", root, node);
  return RecordItem(
      subindex, offset, move(type), move(name), move(access), move(desc));
}

RecordT decodeRecord(const xml_node& root, const xml_node& node) {
  RecordT::Records records;
  for (auto node_value : node.children("RecordItem ")) {
    records.emplace(decodeRecordItem(root, node_value));
  }
  return RecordT(node.attribute("bitLength").as_llong(),
      node.attribute("subindexAccessSupported").as_bool(true), move(records));
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

DataValue decodeDataValue(
    const xml_node& root, const xml_node& node, Datatype type) {
  switch (type) {
  case Datatype::Array: {
    return decodeArray(root, node);
  }
  case Datatype::Record: {
    return decodeRecord(root, node);
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

Repository::VariablesMap decodeStdVariables(const filesystem::path& path) {
  Repository::VariablesMap result;
  auto xml = getXML(path).child("IODDStandardUnitDefinitions");
  auto datatype_collection = xml.child("DatatypeCollection");
  for (auto datatype : datatype_collection.children("Datatype")) {
    result.emplace(datatype.attribute("id").as_string(),
        decodeDataValue(xml, datatype,
            toDatatype(datatype.attribute("xsi:type").as_string())));
  }
  return result;
}

Repository::DescriptorsMap decodeDescriptors(const filesystem::path& path) {
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

  auto descriptors_dir = config_dir / "descriptors";
  auto std_units_filepath = config_dir / "IODD-StandardDefinitions1.1.xml";
  auto std_variables_filepath =
      config_dir / "IODD-StandardUnitDefinitions1.1.1.xml";
  return Repository(decodeDescriptors(descriptors_dir),
      decodeUnits(std_units_filepath),
      decodeStdVariables(std_variables_filepath));
}
} // namespace IODD