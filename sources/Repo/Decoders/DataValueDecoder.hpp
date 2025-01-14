#ifndef __IODD_DATA_VALUE_DECODER_HPP
#define __IODD_DATA_VALUE_DECODER_HPP

#include "Datatypes.hpp"

#include <pugixml.hpp>
#include <stdexcept>
#include <variant>

namespace IODD {

template <class T>
T decodeSimpleDataValue(
    const pugi::xml_node& /* node */, const pugi::xml_node& /* locales */) {
  throw std::runtime_error(
      "Failed to decode Simple Data Value. Unsupported data type");
}

template <>
BooleanT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
UIntegerT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
IntegerT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
FloatT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
StringT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
OctetStringT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
TimeT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
TimeSpanT_Ptr decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

SimpleDatatype decodeSimpleDataValue(
    Datatype type, const pugi::xml_node& node, const pugi::xml_node& locales);

ArrayT_Ptr decodeArrayValue(const DatatypesMap& datatypes_map,
    const pugi::xml_node& node,
    const pugi::xml_node& locale);

RecordT_Ptr decodeRecordValue(const DatatypesMap& datatypes_map,
    const pugi::xml_node& node,
    const pugi::xml_node& locales);

Datatype decodeDatatype(
    const pugi::xml_node& xml, const DatatypesMap& datatypes_map = {});

DatatypesMap decodeDatatypes(const pugi::xml_node& xml,
    const pugi::xml_node& locales,
    const DatatypesMap& std_datatypes = {});

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

struct ProcessDataUnionAsValue : std::runtime_error {
  ProcessDataUnionAsValue()
      : runtime_error("Data Value decoded as Process Data Union") {}
};

DataValue decodeDataValue(const pugi::xml_node& node,
    const pugi::xml_node& locales,
    const DatatypesMap& datatypes_map = {});
} // namespace IODD
#endif //__IODD_DATA_VALUE_DECODER_HPP
