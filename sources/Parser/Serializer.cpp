#include "Serializer.hpp"
#include "XML_Helper.hpp"

#include <cstring>
#include <filesystem>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace pugi;

namespace IODD {

TextID decodeLocalization(const xml_node& locales, const string& text_id) {
  string localization =
      locales.find_child_by_attribute("Text", "id", text_id.c_str())
          .attribute("value")
          .as_string();
  return TextID(text_id, localization);
}

UnitsMapPtr decodeUnits(const filesystem::path& path) {
  auto result = make_shared<UnitsMap>();

  auto doc = getXML(path);
  auto xml = getXMLNode("IODDStandardUnitDefinitions", doc, path);
  auto locales = getXMLNode(
      vector<string>{"ExternalTextCollection", "PrimaryLanguage"}, xml, path);
  auto units_collection = getXMLNode("UnitCollection", xml, path);

  for (const auto& unit : units_collection.children("Unit")) {
    try {
      auto code = getXMLAttribute("code", unit).as_uint();
      string abbr = getXMLAttribute("abbr", unit).as_string();
      string id = getXMLAttribute("textId", unit).as_string();
      result->emplace(
          code, make_shared<Unit>(code, abbr, decodeLocalization(locales, id)));
    } catch (const AttributeNotFound& ex) {
      // @todo: handle attribute not found
    }
  }
  return result;
}

optional<AccessRights> decodeAccessRights(const xml_node& node) {
  try {
    string access_string = node.attribute("accessRights").as_string();
    if (access_string.empty()) {
      access_string =
          getXMLAttribute("accessRightRestriction", node).as_string();
    }
    if (access_string == "ro") {
      return AccessRights::ReadOnly;
    } else if (access_string == "wo") {
      return AccessRights::WriteOnly;
    } else if (access_string == "rw") {
      return AccessRights::ReadWrite;
    } else {
      return nullopt; // @todo: handle malformed access string
    }
  } catch (const AttributeNotFound& ex) {
    return nullopt;
  }
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
optional<TextID> decodeLocalizedText(
    const string& child_name, const xml_node& node, const xml_node& locales) {
  auto name_node = node.child(child_name.c_str());
  if (!name_node.empty()) {
    try {
      return decodeLocalization(
          locales, getXMLAttribute("textId", name_node).as_string());
    } catch (const exception& ex) {
      throw runtime_error(
          "Could not get localization for " + child_name + " field");
    }
  }
  return nullopt;
}

unordered_set<SingleValuePtr<bool>> decodeBoolSingleValues(
    const xml_node& node, const xml_node& locales) {
  unordered_set<SingleValuePtr<bool>> result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<bool>>(
        getXMLAttribute("value", node_value).as_bool(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<uint64_t>::SingleValues decodeUintSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<uint64_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<uint64_t>>(
        getXMLAttribute("value", node_value).as_ullong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<uint64_t>::ValueRanges decodeUintValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<uint64_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<uint64_t>>(
        getXMLAttribute("lowerValue", node_value).as_ullong(),
        getXMLAttribute("upperValue", node_value).as_ullong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<int64_t>::SingleValues decodeIntSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<int64_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<int64_t>>(
        getXMLAttribute("value", node_value).as_llong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<int64_t>::ValueRanges decodeIntValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<int64_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<int64_t>>(
        getXMLAttribute("lowerValue", node_value).as_llong(),
        getXMLAttribute("upperValue", node_value).as_llong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<float>::SingleValues decodeFloatSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<float>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<float>>(
        getXMLAttribute("value", node_value).as_float(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<float>::ValueRanges decodeFloatValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<float>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<float>>(
        getXMLAttribute("lowerValue", node_value).as_float(),
        getXMLAttribute("upperValue", node_value).as_float(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

template <typename T>
NumberT<T> decodeNumericValues(
    const xml_node& /* node */, const xml_node& /* locales */) {
  throw runtime_error("Failed to decode Numeric Value. Unsupported data type");
}

template <>
NumberT<uint64_t> decodeNumericValues(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeUintSingleValues(node, locales);
  auto ranges = decodeUintValueRanges(node, locales);
  return NumberT(move(values), move(ranges));
}

template <>
NumberT<int64_t> decodeNumericValues(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeIntSingleValues(node, locales);
  auto ranges = decodeIntValueRanges(node, locales);
  return NumberT(move(values), move(ranges));
}

template <>
NumberT<float> decodeNumericValues(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeFloatSingleValues(node, locales);
  auto ranges = decodeFloatValueRanges(node, locales);
  return NumberT(move(values), move(ranges));
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
  auto values = decodeNumericValues<uint64_t>(node, locales);
  try {
    auto length = getXMLAttribute("bitLength", node).as_uint();
    return UIntegerT(length, move(values));
  } catch (const AttributeNotFound&) {
    // used to create an update value
    return UIntegerT(move(values));
  }
}

template <>
IntegerT decodeSimpleDataValue(const xml_node& node, const xml_node& locales) {
  auto values = decodeNumericValues<int64_t>(node, locales);
  try {
    auto length = getXMLAttribute("bitLength", node).as_uint();
    return IntegerT(length, move(values));
  } catch (const AttributeNotFound&) {
    // used to create an update value
    return IntegerT(move(values));
  }
}

template <>
FloatT decodeSimpleDataValue(const xml_node& node, const xml_node& locales) {
  return FloatT(decodeNumericValues<float>(node, locales));
}

template <>
StringT decodeSimpleDataValue(
    const xml_node& node, const xml_node& /* locales */) {
  return StringT(getXMLAttribute("fixedLength", node).as_uint(),
      (strcmp(node.attribute("encoding").as_string(), "UTF-8") != 0));
}

template <>
OctetStringT decodeSimpleDataValue(
    const xml_node& node, const xml_node& /* locales */) {
  return OctetStringT(getXMLAttribute("fixedLength", node).as_uint());
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
  Datatype type;
  try {
    string type_string =
        getXMLAttribute("xsi:type", getXMLNode("SimpleDatatype", node))
            .as_string();
    type = toDatatype(type_string);
  } catch (const NodeNotFound& ex) {
    string datatype_ref_id =
        getXMLAttribute("datatypeId", getXMLNode("DatatypeRef", node))
            .as_string();
    auto it = datatypes_map.find(datatype_ref_id);
    if (it != datatypes_map.end()) {
      type = toDatatype(it->second);
    } else {
      throw runtime_error("Failed to decode RecordT. Datatype " +
          datatype_ref_id + " is not defined");
    }
  }

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

  auto name_locale = decodeLocalizedText("Name", node, locales);
  if (!name_locale.has_value()) {
    throw runtime_error("Missing RecordItem name localization");
  }

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
  auto access = decodeAccessRights(node);
  auto desc = decodeLocalizedText("Description", node, locales);

  return RecordItem(subindex,
      offset,
      move(value),
      move(name_locale.value()),
      access,
      move(desc));
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
  return RecordT<T>(getXMLAttribute("bitLength", node).as_llong(),
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
  Datatype type;
  try {
    string type_string = getXMLAttribute(
        "xsi:type", vector<string>{"RecordItem", "SimpleDatatype"}, node)
                             .as_string();
    type = toDatatype(type_string);
  } catch (const NodeNotFound& ex) {
    string datatype_ref_id = getXMLAttribute(
        "datatypeId", vector<string>{"RecordItem", "DatatypeRef"}, node)
                                 .as_string();
    auto it = datatypes_map.find(datatype_ref_id);
    if (it != datatypes_map.end()) {
      type = toDatatype(it->second);
    } else {
      throw runtime_error("Failed to decode RecordT. Datatype " +
          datatype_ref_id + " is not defined");
    }
  }

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

Datatype decodeDatatype(
    const xml_node& xml, const Repository::DatatypesMap& datatypes_map = {}) {
  string type_string = getXMLAttribute("xsi:type", xml).as_string();
  if (type_string == "RecordT" || type_string == "ArrayT") {
    xml_node node;
    if (type_string == "RecordT") {
      node = getXMLNode("RecordItem", xml);
    } else {
      node = xml;
    }

    string subtype_string;
    try {
      subtype_string =
          getXMLAttribute("xsi:type", getXMLNode("SimpleDatatype", node))
              .as_string();
    } catch (const NodeNotFound&) {
      string datatype_ref_id =
          getXMLAttribute("datatypeId", getXMLNode("DatatypeRef", node))
              .as_string();
      auto it = datatypes_map.find(datatype_ref_id);
      if (it != datatypes_map.end()) {
        subtype_string = toString(toDatatype(it->second));
      } else {
        throw runtime_error("Failed to decode " + type_string +
            " subtype. DatatypeRef " + datatype_ref_id + " is not defined");
      }
    }
    return toDatatype(subtype_string + "_" + type_string);
  } else {
    return toDatatype(type_string);
  }
}

DataValue decodeDataValue(const xml_node& node,
    const xml_node& locales,
    const Repository::DatatypesMap& datatypes_map = {}) {
  auto type = decodeDatatype(node, datatypes_map);
  if (isArray(type)) {
    return variantCast(decodeArrayValue(datatypes_map, node, locales));
  } else if (isRecord(type)) {
    return variantCast(decodeRecordValue(datatypes_map, node, locales));
  } else if (type == Datatype::ProcessDataIn) {
    return ProcessDataIn();
  } else if (type == Datatype::ProcessDataOut) {
    return ProcessDataOut();
  } else {
    return variantCast(decodeSimpleDataValue(type, node, locales));
  }
}

Repository::DatatypesMap decodeDatatypes(const xml_node& xml,
    const xml_node& locales,
    const Repository::DatatypesMap& std_datatypes = {}) {
  Repository::DatatypesMap datatypes = std_datatypes;
  for (auto datatype : xml.children("Datatype")) {
    string id = getXMLAttribute("id", datatype).as_string();
    datatypes.emplace(id, decodeDataValue(datatype, locales));
  }
  return datatypes;
}

VariablesMap decodeVariables(const xml_node& xml,
    const xml_node& locales,
    const Repository::DatatypesMap& datatypes,
    const VariablesMap& std_variables = {}) {
  VariablesMap variables = std_variables;
  for (auto variable : xml.children("Variable")) {
    string id = getXMLAttribute("id", variable).as_string();
    auto index = getXMLAttribute("index", variable).as_ullong();

    auto name_locale = decodeLocalizedText("Name", variable, locales);
    if (!name_locale.has_value()) {
      throw runtime_error("Variable " + id + " does no have name localization");
    }

    auto access = decodeAccessRights(variable);
    if (!access.has_value()) {
      throw runtime_error(
          "Variable " + id + " does no have access rights specification");
    }

    try {
      auto description = decodeLocalizedText("Description", variable, locales);
      auto dynamic = variable.attribute("dynamic").as_bool(false);
      auto modifies_others =
          variable.attribute("modifiesOtherVariables").as_bool(false);
      auto historized =
          variable.attribute("excludedFromDataStorage").as_bool(false);
      DataValue data_value;
      try {
        auto datatype_xml = getXMLNode("Datatype", variable);
        data_value = decodeDataValue(datatype_xml, locales, datatypes);
      } catch (const NodeNotFound& ex) {
        string datatype_ref_id =
            getXMLAttribute("datatypeId", getXMLNode("DatatypeRef", variable))
                .as_string();
        auto it = datatypes.find(datatype_ref_id);
        if (it != datatypes.end()) {
          data_value = it->second;
        } else {
          // NOLINTNEXTLINE(performance-inefficient-string-concatenation)
          throw runtime_error("Variable " + id + " requires DataTypeRef " +
              datatype_ref_id + ", but it does not exist");
        }
      }
      variables.emplace(id,
          make_shared<Variable>(index,
              move(name_locale.value()),
              access.value(),
              move(data_value),
              move(description),
              nullopt,
              dynamic,
              modifies_others,
              historized));
    } catch (const exception& ex) {
      throw runtime_error("Could not process variable " + id +
          " due to exception: " + ex.what());
    }
  }
  return variables;
}

pair<Repository::DatatypesMapPtr, VariablesMapPtr> decodeStdDefinitions(
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

    return make_pair(make_shared<Repository::DatatypesMap>(datatypes),
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

SimpleDatatypeValue decodeDefaultValue(
    Datatype type, const xml_attribute& attribute) {
  switch (type) {
  case Datatype::Boolean: {
    return attribute.as_bool();
  }
  case Datatype::UInteger: {
    return (uint64_t)attribute.as_ullong();
  }
  case Datatype::Integer: {
    return (int64_t)attribute.as_llong();
  }
  case Datatype::Float32: {
    return attribute.as_float();
  }
    // NOLINTNEXTLINE(bugprone-branch-clone)
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
    const Repository::DatatypesMapPtr& /* datatypes */,
    const xml_node& node,
    const xml_node& locales) {
  if (!node.children().empty()) {
    if (type == IODD::Datatype::Boolean || isNumericData(type)) {
      auto simple_value = decodeSimpleDataValue(type, node, locales);
      return variantCast(simple_value);
    } else {
      // @TODO:handle Complex data types like StdRecordItemRef here
    }
  }
  return nullopt;
}

VariablesMap decodeStdVariables(const xml_node& xml,
    const xml_node& locales,
    const Repository::DatatypesMapPtr& datatypes,
    const VariablesMapPtr& std_variables) {
  VariablesMap variables = *std_variables;
  for (auto variable : xml.children("StdVariableRef")) {
    string id = variable.attribute("id").as_string();
    // ignore parameter overlays
    try {
      if (id != "V_DirectParameters_1" && id != "V_DirectParameters_2") {
        if (auto std_variable = variables.find(id);
            std_variable != variables.end()) {
          optional<SimpleDatatypeValue> default_value = nullopt;
          if (auto node_value = variable.attribute("defaultValue");
              !node_value.empty() &&
              // @TODO: implement support for nonSimpleData default values
              isSimpleData(std_variable->second->type())) {
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
    } catch (const exception& ex) {
      throw runtime_error("Failed to decode StdVariableRef " + id +
          " due to exception: " + ex.what());
    }
  }
  return variables;
}

unordered_map<string, string> decodeMenuIDs(const xml_node& xml) {
  unordered_map<string, string> result;
  for (const auto& child : xml.children()) {
    if (!child.attribute("menuId").empty()) {
      result.emplace(child.name(), child.attribute("menuId").as_string());
    }
  }
  return result;
}

VariablePtr findVariable(const string& id,
    const VariablesMapPtr& variables,
    const VariablesMapPtr& std_variables) {
  auto it = variables->find(id);
  if (it != variables->end()) {
    return it->second;
  }

  it = std_variables->find(id);
  if (it != std_variables->end()) {
    return it->second;
  }

  throw out_of_range(id + " variable does not exist");
}

UnitPtr findUnit(uint16_t id, const UnitsMapPtr& units) {
  auto it = units->find(id);
  if (it != units->end()) {
    return it->second;
  }
  return nullptr;
}

optional<float> decodeFloatAttribute(const xml_node& xml, const string& name) {
  auto attribute = xml.attribute(name.c_str());
  if (!attribute.empty()) {
    return attribute.as_float();
  }
  return nullopt;
}

UnitPtr decodeUnitPtr(const UnitsMapPtr& units, const xml_node& xml) {
  auto unit_attribute = xml.attribute("unitCode");
  if (!unit_attribute.empty()) {
    auto unit_id = unit_attribute.as_uint();
    return findUnit(unit_id, units);
  }
  return nullptr;
}

DisplayFormat decodeDisplayFormat(const xml_node& xml) {
  auto format_attribute = xml.attribute("displayFormat");
  if (!format_attribute.empty()) {
    unordered_map<string, DisplayFormat> format_types{
        {"Dec.0", DisplayFormat::Dec0},
        {"Dec.1", DisplayFormat::Dec1},
        {"Dec.2", DisplayFormat::Dec2},
        {"Dec.3", DisplayFormat::Dec3},
        {"Dec.4", DisplayFormat::Dec4},
        {"Dec.5", DisplayFormat::Dec5},
        {"Dec.6", DisplayFormat::Dec6},
        {"Dec.7", DisplayFormat::Dec7},
        {"Dec.8", DisplayFormat::Dec8},
        {"Dec.9", DisplayFormat::Dec9},
        {"Bin", DisplayFormat::Bin},
        {"Hex", DisplayFormat::Hex}};
    return format_types.at(format_attribute.as_string());
  }
  return DisplayFormat::None;
}

VariableRef::ButtonValue decodeButtonValue(
    Datatype type, const xml_attribute& xml) {
  switch (type) {
  case Datatype::Boolean: {
    return xml.as_bool();
  }
  case Datatype::Integer: {
    return (int64_t)xml.as_llong();
  }
  case Datatype::UInteger: {
    return (uint64_t)xml.as_ullong();
  }
  default: {
    throw invalid_argument(toString(type) + " can not be used as button value");
  }
  }
}

VariableRefPtr decodeVariableRef(const UnitsMapPtr& units,
    const VariablesMapPtr& variables,
    const VariablesMapPtr& std_variables,
    const xml_node& xml,
    const xml_node& locales) {
  string variable_id = getXMLAttribute("variableId", xml).as_string();
  auto variable = findVariable(variable_id, variables, std_variables);

  try {
    auto button_xml = getXMLNode("Button", xml);
    auto button_value = decodeButtonValue(
        variable->type(), button_xml.attribute("buttonValue"));
    auto description = decodeLocalizedText("Description", button_xml, locales);
    auto action_msg =
        decodeLocalizedText("ActionStartedMessage", button_xml, locales);

    return make_shared<VariableRef>(
        variable, button_value, description, action_msg);
  } catch (const NodeNotFound& ex) {
    auto gradient = decodeFloatAttribute(xml, "gradient");
    auto offset = decodeFloatAttribute(xml, "offset");
    auto unit = decodeUnitPtr(units, xml);
    auto format = decodeDisplayFormat(xml);
    auto access = decodeAccessRights(xml);

    return make_shared<VariableRef>(
        variable, gradient, offset, unit, format, access);
  }
}

RecordRefPtr decodeRecordRef(const UnitsMapPtr& units,
    const VariablesMapPtr& variables,
    const VariablesMapPtr& std_variables,
    const xml_node& xml,
    const xml_node& locales) {
  string variable_id = getXMLAttribute("variableId", xml).as_string();
  auto variable = findVariable(variable_id, variables, std_variables);
  auto subindex = getXMLAttribute("subindex", xml).as_uint();

  try {
    auto button_xml = getXMLNode("Button", xml);
    auto button_value = decodeButtonValue(
        variable->type(), button_xml.attribute("buttonValue"));
    auto description = decodeLocalizedText("Description", button_xml, locales);
    auto action_msg =
        decodeLocalizedText("ActionStartedMessage", button_xml, locales);

    return make_shared<RecordRef>(
        variable, button_value, description, action_msg, subindex);
  } catch (const NodeNotFound& ex) {
    auto gradient = decodeFloatAttribute(xml, "gradient");
    auto offset = decodeFloatAttribute(xml, "offset");
    auto unit = decodeUnitPtr(units, xml);
    auto format = decodeDisplayFormat(xml);
    auto access = decodeAccessRights(xml);

    return make_shared<RecordRef>(
        variable, gradient, offset, unit, format, access, subindex);
  }
}

MenuPtr decodeMenu(const UnitsMapPtr& units,
    const VariablesMapPtr& variables,
    const VariablesMapPtr& std_variables,
    const string& menu_id,
    const xml_node& menus,
    const xml_node& locales,
    const optional<Condition>& condition = nullopt) {
  auto xml = menus.find_child_by_attribute("Menu", "id", menu_id.c_str());

  if (strcmp(xml.name(), "Menu") != 0) {
    throw logic_error(
        "Can not decode " + menu_id + " as a Menu element. It's not a Menu");
  }

  std::vector<Menu::Ref> refs;
  optional<TextID> name = decodeLocalizedText("Name", xml, locales);

  for (const auto& child : xml.children()) {
    if (strcmp(child.name(), "VariableRef") == 0) {
      refs.emplace_back(
          decodeVariableRef(units, variables, std_variables, child, locales));
    }
    if (strcmp(child.name(), "RecordItemRef") == 0) {
      refs.emplace_back(
          decodeRecordRef(units, variables, std_variables, child, locales));
    }
    if (strcmp(child.name(), "MenuRef") == 0) {
      string ref_id = getXMLAttribute("menuId", child).as_string();
      optional<Condition> ref_condition = nullopt;
      auto child_condition = child.child("Condition");
      if (!child_condition.empty()) {
        VariablePtr condition_variable =
            findVariable(child_condition.attribute("variableId").as_string(),
                variables,
                std_variables);

        std::optional<uint8_t> condition_subindex = nullopt;
        auto subindex_attribute = child_condition.attribute("subindex");
        if (subindex_attribute.empty()) {
          condition_subindex = subindex_attribute.as_uint();
        }

        uint8_t condition_value =
            getXMLAttribute("value", child_condition).as_uint();

        ref_condition =
            Condition(condition_variable, condition_subindex, condition_value);
      }
      refs.emplace_back(decodeMenu(units,
          variables,
          std_variables,
          ref_id,
          menus,
          locales,
          ref_condition));
    }
  }
  return make_shared<Menu>(menu_id, refs, name, condition);
}

MenuPtr decodeOptionalMenu(const string& menu_name,
    unordered_map<string, string> menu_ids,
    const UnitsMapPtr& units,
    const VariablesMapPtr& variables,
    const VariablesMapPtr& std_variables,
    const xml_node& xml,
    const xml_node& locales) {
  try {
    return decodeMenu(
        units, variables, std_variables, menu_ids.at(menu_name), xml, locales);
  } catch (const out_of_range& ex) {
    return nullptr;
  }
}

UserInterfacePtr decodeRoleUI(UserRole role,
    unordered_map<string, string> menu_ids,
    const UnitsMapPtr& units,
    const VariablesMapPtr& variables,
    const VariablesMapPtr& std_variables,
    const xml_node& xml,
    const xml_node& locales) {
  auto ident_menu = decodeMenu(units,
      variables,
      std_variables,
      menu_ids.at("IdentificationMenu"),
      xml,
      locales);
  auto param_menu = decodeOptionalMenu(
      "ParameterMenu", menu_ids, units, variables, std_variables, xml, locales);
  auto observe_menu = decodeOptionalMenu("ObservationMenu",
      menu_ids,
      units,
      variables,
      std_variables,
      xml,
      locales);
  auto diag_menu = decodeOptionalMenu(
      "DiagnosisMenu", menu_ids, units, variables, std_variables, xml, locales);

  return make_shared<UserInterface>(
      role, ident_menu, param_menu, observe_menu, diag_menu);
}

unordered_map<UserRole, UserInterfacePtr> decodeUI(const UnitsMapPtr& units,
    const VariablesMapPtr& variables,
    const VariablesMapPtr& std_variables,
    const xml_node& xml,
    const xml_node& locales) {
  unordered_map<UserRole, UserInterfacePtr> result;

  auto menus_xml = getXMLNode("MenuCollection", xml);

  auto observer_menu_ids =
      decodeMenuIDs(getXMLNode("ObserverRoleMenuSet", xml));
  result.emplace(UserRole::ObservationRole,
      decodeRoleUI(UserRole::ObservationRole,
          observer_menu_ids,
          units,
          variables,
          std_variables,
          menus_xml,
          locales));

  auto maintainence_menu_ids =
      decodeMenuIDs(getXMLNode("MaintenanceRoleMenuSet", xml));
  result.emplace(UserRole::MaintenanceRole,
      decodeRoleUI(UserRole::MaintenanceRole,
          maintainence_menu_ids,
          units,
          variables,
          std_variables,
          menus_xml,
          locales));

  auto specialist_menu_ids =
      decodeMenuIDs(getXMLNode("SpecialistRoleMenuSet", xml));
  result.emplace(UserRole::SpecialistRole,
      decodeRoleUI(UserRole::SpecialistRole,
          specialist_menu_ids,
          units,
          variables,
          std_variables,
          menus_xml,
          locales));

  return result;
}

// NOLINTEND(bugprone-easily-swappable-parameters)

DeviceDescriptorPtr decode(const UnitsMapPtr& units,
    const Repository::DatatypesMapPtr& std_datatypes,
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
    const pair<Repository::DatatypesMapPtr, VariablesMapPtr>& variables,
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

Repository deserializeModel(const string& config_directory_path) {
  auto config_dir = filesystem::path(config_directory_path);

  if (!filesystem::is_directory(config_dir)) {
    throw invalid_argument(config_dir.string() + " is not a directory");
  }

  auto std_units_map =
      decodeUnits(config_dir / "IODD-StandardUnitDefinitions.xml");
  auto std_variables_map =
      decodeStdDefinitions(config_dir / "IODD-StandardDefinitions.xml");
  auto descriptors = decodeDescriptors(
      std_units_map, std_variables_map, config_dir / "descriptors");
  return Repository(
      move(std_units_map), move(std_variables_map), move(descriptors));
}
} // namespace IODD