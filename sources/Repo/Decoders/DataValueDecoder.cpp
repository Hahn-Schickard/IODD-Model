#include "DataValueDecoder.hpp"
#include "NumericsDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "XML_Helper.hpp"

#include <cstring>
#include <string>

using namespace std;
using namespace pugi;

namespace IODD {
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

template <typename T>
ArrayT<T> decodeArray(const DatatypesMap& datatypes_map,
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
RecordItem<T> decodeRecordItem(const DatatypesMap& datatypes_map,
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
RecordT<T> decodeRecord(const DatatypesMap& datatypes_map,
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

RecordValue decodeRecordValue(const DatatypesMap& datatypes_map,
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

Datatype decodeDatatype(
    const xml_node& xml, const DatatypesMap& datatypes_map) {
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
    const DatatypesMap& datatypes_map) {
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
} // namespace IODD
