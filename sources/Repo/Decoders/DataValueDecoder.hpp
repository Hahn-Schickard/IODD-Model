#ifndef __IODD_DATA_VALUE_DECODER_HPP
#define __IODD_DATA_VALUE_DECODER_HPP

#include "Array.hpp"
#include "Boolean.hpp"
#include "Datatypes.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "OctetString.hpp"
#include "Record.hpp"
#include "String.hpp"
#include "Time.hpp"
#include "Timespan.hpp"
#include "UInteger.hpp"

#include <pugixml.hpp>
#include <stdexcept>
#include <variant>

namespace IODD {

template <typename T>
T decodeSimpleDataValue(
    const pugi::xml_node& /* node */, const pugi::xml_node& /* locales */) {
  throw std::runtime_error(
      "Failed to decode Simple Data Value. Unsupported data type");
}

template <>
BooleanT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
UIntegerT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
IntegerT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
FloatT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
StringT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
OctetStringT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
TimeT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
TimeSpanT decodeSimpleDataValue(
    const pugi::xml_node& node, const pugi::xml_node& locales);

SimpleDatatype decodeSimpleDataValue(
    Datatype type, const pugi::xml_node& node, const pugi::xml_node& locales);

using ArrayValue = std::variant<ArrayT<BooleanT>,
    ArrayT<UIntegerT>,
    ArrayT<IntegerT>,
    ArrayT<FloatT>,
    ArrayT<OctetStringT>,
    ArrayT<StringT>,
    ArrayT<TimeT>,
    ArrayT<TimeSpanT>>;

ArrayValue decodeArrayValue(const DatatypesMap& datatypes_map,
    const pugi::xml_node& node,
    const pugi::xml_node& locale);

using RecordValue = std::variant<RecordT<BooleanT>,
    RecordT<UIntegerT>,
    RecordT<IntegerT>,
    RecordT<FloatT>,
    RecordT<OctetStringT>,
    RecordT<StringT>,
    RecordT<TimeT>,
    RecordT<TimeSpanT>>;

RecordValue decodeRecordValue(const DatatypesMap& datatypes_map,
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
