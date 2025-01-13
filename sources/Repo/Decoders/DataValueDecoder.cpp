#include "DataValueDecoder.hpp"
#include "NumericsDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "XML_Helper.hpp"

#include <Variant_Visitor.hpp>

#include <cstring>
#include <string>

using namespace std;
using namespace pugi;

namespace IODD {
template <>
BooleanT_Ptr decodeSimpleDataValue(
    const xml_node& node, const xml_node& locales) {
  return make_shared<BooleanT>(decodeBoolSingleValues(node, locales));
}

template <>
UIntegerT_Ptr decodeSimpleDataValue(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeNumericValues<uint64_t>(node, locales);
  try {
    auto length = getXMLAttribute("bitLength", node).as_uint();
    return make_shared<UIntegerT>(length, move(values));
  } catch (const AttributeNotFound&) {
    // used to create an update value
    return make_shared<UIntegerT>(move(values));
  }
}

template <>
IntegerT_Ptr decodeSimpleDataValue(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeNumericValues<int64_t>(node, locales);
  try {
    auto length = getXMLAttribute("bitLength", node).as_uint();
    return make_shared<IntegerT>(length, move(values));
  } catch (const AttributeNotFound&) {
    // used to create an update value
    return make_shared<IntegerT>(move(values));
  }
}

template <>
FloatT_Ptr decodeSimpleDataValue(
    const xml_node& node, const xml_node& locales) {
  return make_shared<FloatT>(decodeNumericValues<float>(node, locales));
}

template <>
StringT_Ptr decodeSimpleDataValue(
    const xml_node& node, const xml_node& /* locales */) {
  return make_shared<StringT>(getXMLAttribute("fixedLength", node).as_uint(),
      (strcmp(node.attribute("encoding").as_string(), "UTF-8") != 0));
}

template <>
OctetStringT_Ptr decodeSimpleDataValue(
    const xml_node& node, const xml_node& /* locales */) {
  return make_shared<OctetStringT>(
      getXMLAttribute("fixedLength", node).as_uint());
}

template <>
TimeT_Ptr decodeSimpleDataValue(
    const xml_node& /* node */, const xml_node& /* locales */) {
  return make_shared<TimeT>();
}

template <>
TimeSpanT_Ptr decodeSimpleDataValue(
    const xml_node& /* node */, const xml_node& /* locales */) {
  return make_shared<TimeSpanT>();
}

SimpleDatatype decodeSimpleDataValue(
    Datatype type, const xml_node& node, const xml_node& locales) {
  switch (type) {
  case Datatype::Boolean: {
    return decodeSimpleDataValue<BooleanT_Ptr>(node, locales);
  }
  case Datatype::UInteger: {
    return decodeSimpleDataValue<UIntegerT_Ptr>(node, locales);
  }
  case Datatype::Integer: {
    return decodeSimpleDataValue<IntegerT_Ptr>(node, locales);
  }
  case Datatype::Float32: {
    return decodeSimpleDataValue<FloatT_Ptr>(node, locales);
  }
  case Datatype::String: {
    return decodeSimpleDataValue<StringT_Ptr>(node, locales);
  }
  case Datatype::OctetString: {
    return decodeSimpleDataValue<OctetStringT_Ptr>(node, locales);
  }
  case Datatype::Time: {
    return decodeSimpleDataValue<TimeT_Ptr>(node, locales);
  }
  case Datatype::TimeSpan: {
    return decodeSimpleDataValue<TimeSpanT_Ptr>(node, locales);
  default: {
    throw invalid_argument(toString(type) + " is not a simple data type");
  }
  }
  }
}

template <typename T>
ArrayT_Ptr<T> decodeArray(const DatatypesMap& datatypes_map,
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

  return make_shared<ArrayT<T>>(node.attribute("count").as_llong(),
      node.attribute("subindexAccessSupported").as_bool(true),
      move(values));
}

ArrayValue decodeArrayValue(const DatatypesMap& datatypes_map,
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
    return decodeArray<BooleanT_Ptr>(datatypes_map, node, locale);
  }
  case Datatype::UInteger: {
    return decodeArray<UIntegerT_Ptr>(datatypes_map, node, locale);
  }
  case Datatype::Integer: {
    return decodeArray<IntegerT_Ptr>(datatypes_map, node, locale);
  }
  case Datatype::Float32: {
    return decodeArray<FloatT_Ptr>(datatypes_map, node, locale);
  }
  case Datatype::String: {
    return decodeArray<StringT_Ptr>(datatypes_map, node, locale);
  }
  case Datatype::OctetString: {
    return decodeArray<OctetStringT_Ptr>(datatypes_map, node, locale);
  }
  case Datatype::Time: {
    return decodeArray<TimeT_Ptr>(datatypes_map, node, locale);
  }
  case Datatype::TimeSpan: {
    return decodeArray<TimeSpanT_Ptr>(datatypes_map, node, locale);
  }
  default: {
    throw runtime_error("Failed to decode ArrayT. " + toString(type) +
        " is not a simple data type");
  }
  }
}

SimpleDatatype getSimpleDatatype(DataValue value) {
  SimpleDatatype result;
  match(
      value,
      [&result](const BooleanT_Ptr& value) { result = value; },
      [&result](const UIntegerT_Ptr& value) { result = value; },
      [&result](const IntegerT_Ptr& value) { result = value; },
      [&result](const FloatT_Ptr& value) { result = value; },
      [&result](const OctetStringT_Ptr& value) { result = value; },
      [&result](const StringT_Ptr& value) { result = value; },
      [&result](const TimeT_Ptr& value) { result = value; },
      [&result](const TimeSpanT_Ptr& value) { result = value; },
      [&result](
          const auto&) { throw runtime_error("Not a simple data type"); });
  return result;
}

RecordItem_Ptr decodeRecordItem(const DatatypesMap& datatypes_map,
    const xml_node& node,
    const xml_node& locales) {
  SimpleDatatype value;

  auto name_locale = decodeLocalizedText("Name", node, locales);
  if (!name_locale.has_value()) {
    throw runtime_error("Missing RecordItem name localization");
  }

  try {
    string type_string =
        getXMLAttribute("xsi:type", vector<string>{"SimpleDatatype"}, node)
            .as_string();
    auto type = toDatatype(type_string);
    value = decodeSimpleDataValue(type, node, locales);
  } catch (const NodeNotFound& ex) {
    string datatype_ref_id =
        getXMLAttribute("datatypeId", vector<string>{"DatatypeRef"}, node)
            .as_string();
    auto it = datatypes_map.find(datatype_ref_id);
    if (it != datatypes_map.end()) {
      value = getSimpleDatatype(it->second);
    } else {
      throw runtime_error("Failed to decode RecordT. Datatype " +
          datatype_ref_id + " is not defined");
    }
  }

  auto subindex = node.attribute("subindex").as_ullong();
  auto offset = node.attribute("bitOffset").as_ullong();
  auto access = decodeAccessRights(node);
  auto desc = decodeLocalizedText("Description", node, locales);
  return make_shared<RecordItem>(subindex,
      offset,
      move(value),
      move(name_locale.value()),
      access,
      move(desc));
}

RecordT_Ptr decodeRecordValue(const DatatypesMap& datatypes_map,
    const xml_node& node,
    const xml_node& locales) {
  RecordItems records;
  for (auto node_value : node.children("RecordItem")) {
    auto record = decodeRecordItem(datatypes_map, node_value, locales);
    auto subindex = node_value.attribute("subindex").as_ullong();
    records.emplace(subindex, move(record));
  }
  return make_shared<RecordT>(getXMLAttribute("bitLength", node).as_llong(),
      node.attribute("subindexAccessSupported").as_bool(true),
      move(records));
}

Datatype decodeDatatype(
    const xml_node& xml, const DatatypesMap& datatypes_map) {
  string type_string = getXMLAttribute("xsi:type", xml).as_string();
  if (type_string == "ArrayT") {
    string subtype_string;
    try {
      subtype_string =
          getXMLAttribute("xsi:type", getXMLNode("SimpleDatatype", xml))
              .as_string();
    } catch (const NodeNotFound&) {
      string datatype_ref_id =
          getXMLAttribute("datatypeId", getXMLNode("DatatypeRef", xml))
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

DatatypesMap decodeDatatypes(const xml_node& xml,
    const xml_node& locales,
    const DatatypesMap& std_datatypes) {
  DatatypesMap datatypes = std_datatypes;
  for (auto datatype : xml.children("Datatype")) {
    string id = getXMLAttribute("id", datatype).as_string();
    datatypes.emplace(id, decodeDataValue(datatype, locales));
  }
  return datatypes;
}

DataValue decodeDataValue(const xml_node& node,
    const xml_node& locales,
    const DatatypesMap&
        datatypes_map) { // @todo: check if DatatypesMap can be made into ptr
  auto type = decodeDatatype(node, datatypes_map);
  if (isArray(type)) {
    return variantCast(decodeArrayValue(datatypes_map, node, locales));
  } else if (isRecord(type)) {
    return decodeRecordValue(datatypes_map, node, locales);
  } else if (isProcessData(type)) {
    throw ProcessDataUnionAsValue();
  } else {
    return variantCast(decodeSimpleDataValue(type, node, locales));
  }
}
} // namespace IODD
