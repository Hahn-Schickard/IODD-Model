#include "Datatypes.hpp"

#include <Variant_Visitor/Visitor.hpp>

using namespace std;

namespace IODD {
void expand(DataValue& lhs, const DataValue& rhs) {
  if (lhs.index() != rhs.index()) {
    throw logic_error("Expanded data type does not match variable datatype");
  }
  Variant_Visitor::match(
      lhs,
      [&rhs](const BooleanT_Ptr& value) {
        value->expand(*get<BooleanT_Ptr>(rhs));
      },
      [&rhs](const UIntegerT_Ptr& value) {
        value->expand(*get<UIntegerT_Ptr>(rhs));
      },
      [&rhs](const IntegerT_Ptr& value) {
        value->expand(*get<IntegerT_Ptr>(rhs));
      },
      [&rhs](const FloatT_Ptr& value) { value->expand(*get<FloatT_Ptr>(rhs)); },
      [](const OctetStringT_Ptr&) { /* no expansion for OctetStringT */ },
      [](const StringT_Ptr&) { /* no expansion for StringT */ },
      [](const TimeT_Ptr&) { /* no expansion for TimeT */ },
      [](const TimeSpanT_Ptr&) { /* no expansion for TimeSpanT */ },
      // array types
      [&rhs](const ArrayT_Ptr& value) { value->expand(*get<ArrayT_Ptr>(rhs)); },
      // record types
      [&rhs](
          const RecordT_Ptr& value) { value->expand(*get<RecordT_Ptr>(rhs)); });
}

template <typename T>
T getSimpleDatatypeValue(const SimpleDatatypeValue& variant) {
  auto variant_value = variant();
  if (holds_alternative<T>(variant_value)) {
    return get<T>(variant_value);
  } else {
    throw std::invalid_argument(
        "Simple value variant does not hold the requested type");
  }
}

template <typename T>
NamedAttributePtr getValueName(const T&, const SimpleDatatypeValue&) {
  throw std::runtime_error(
      "Given value type does not support named value attribute");
}

template <>
NamedAttributePtr getValueName<BooleanT>(
    const BooleanT& value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<bool>(value));
}

template <>
NamedAttributePtr getValueName<UIntegerT>(
    const UIntegerT& value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<uint64_t>(value));
}

template <>
NamedAttributePtr getValueName<IntegerT>(
    const IntegerT& value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<int64_t>(value));
}

template <>
NamedAttributePtr getValueName<FloatT>(
    const FloatT& value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<float>(value));
}

NamedAttributePtr getValueName(const RecordT& record,
    const optional<uint8_t>& subindex,
    const SimpleDatatypeValue& value) {
  if (record.subindexAccess()) {
    if (subindex.has_value()) {
      auto item = record.item(subindex.value());
      return getValueName(item->value(), value);
    } else {
      throw std::invalid_argument("No subindex provided for record access");
    }
  } else {
    throw std::logic_error("Given record does not support subindex access");
  }
}

NamedAttributePtr getValueName(const DataValue& type,
    const SimpleDatatypeValue& value,
    std::optional<uint8_t> subindex) {
  if (holds_alternative<BooleanT_Ptr>(type)) {
    auto value_type = get<BooleanT_Ptr>(type);
    return getValueName<BooleanT>(*value_type, value);
  } else if (holds_alternative<BooleanT_Ptr>(type)) {
    auto value_type = get<UIntegerT_Ptr>(type);
    return getValueName<UIntegerT>(*value_type, value);
  } else if (holds_alternative<BooleanT_Ptr>(type)) {
    auto value_type = get<IntegerT_Ptr>(type);
    return getValueName<IntegerT>(*value_type, value);
  } else if (holds_alternative<BooleanT_Ptr>(type)) {
    auto value_type = get<FloatT_Ptr>(type);
    return getValueName<FloatT>(*value_type, value);
  } else if (holds_alternative<BooleanT_Ptr>(type)) {
    auto value_type = get<RecordT_Ptr>(type);
    return getValueName(*value_type, subindex, value);
  } else {
    throw logic_error(toString(toDatatype(type)) +
        " does not support named value attribute lookup");
  }
}

Datatype toDatatype(const DataValue& variant) {
  return Variant_Visitor::match(
      variant,
      [](const BooleanT_Ptr&) { return Datatype::Boolean; },
      [](const UIntegerT_Ptr&) { return Datatype::UInteger; },
      [](const IntegerT_Ptr&) { return Datatype::Integer; },
      [](const FloatT_Ptr&) { return Datatype::Float32; },
      [](const OctetStringT_Ptr&) { return Datatype::OctetString; },
      [](const StringT_Ptr&) { return Datatype::String; },
      [](const TimeT_Ptr&) { return Datatype::Time; },
      [](const TimeSpanT_Ptr&) { return Datatype::TimeSpan; },
      // array types
      [](const ArrayT_Ptr&) { return Datatype::Array; },
      // record types
      [](const RecordT_Ptr&) { return Datatype::Record; });
}
} // namespace IODD
