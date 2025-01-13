#include "Datatypes.hpp"

#include "Variant_Visitor.hpp"

using namespace std;

namespace IODD {
void expand(DataValue& lhs, const DataValue& rhs) {
  if (lhs.index() != rhs.index()) {
    throw logic_error("Expanded data type does not match variable datatype");
  }
  match(
      lhs,
      [&rhs](BooleanT_Ptr value) { value->expand(*get<BooleanT_Ptr>(rhs)); },
      [&rhs](UIntegerT_Ptr value) { value->expand(*get<UIntegerT_Ptr>(rhs)); },
      [&rhs](IntegerT_Ptr value) { value->expand(*get<IntegerT_Ptr>(rhs)); },
      [&rhs](FloatT_Ptr value) { value->expand(*get<FloatT_Ptr>(rhs)); },
      [&rhs](OctetStringT_Ptr) { /* no expansion for OctetStringT */ },
      [&rhs](StringT_Ptr) { /* no expansion for StringT */ },
      [&rhs](TimeT_Ptr) { /* no expansion for TimeT */ },
      [&rhs](TimeSpanT_Ptr) { /* no expansion for TimeSpanT */ },
      // array types
      [&rhs](ArrayT_Ptr<BooleanT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<BooleanT_Ptr>>(rhs));
      },
      [&rhs](ArrayT_Ptr<UIntegerT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<UIntegerT_Ptr>>(rhs));
      },
      [&rhs](ArrayT_Ptr<IntegerT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<IntegerT_Ptr>>(rhs));
      },
      [&rhs](ArrayT_Ptr<FloatT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<FloatT_Ptr>>(rhs));
      },
      [&rhs](ArrayT_Ptr<OctetStringT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<OctetStringT_Ptr>>(rhs));
      },
      [&rhs](ArrayT_Ptr<StringT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<StringT_Ptr>>(rhs));
      },
      [&rhs](ArrayT_Ptr<TimeT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<TimeT_Ptr>>(rhs));
      },
      [&rhs](ArrayT_Ptr<TimeSpanT_Ptr> value) {
        value->expand(*get<ArrayT_Ptr<TimeSpanT_Ptr>>(rhs));
      },
      // record types
      [&rhs](RecordT_Ptr value) { value->expand(*get<RecordT_Ptr>(rhs)); });
}

template <typename T>
T getSimpleDatatypeValue(const SimpleDatatypeValue& variant) {
  if (auto* value = std::get_if<T>(&variant)) {
    return *value;
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
  // use raw pointer to avoid shared_ptr memory leak in lambda capture
  NamedAttribute* result = nullptr;
  match(
      type,
      // Simple Types
      [&result, &value](const BooleanT_Ptr& value_type) {
        auto ptr = move(getValueName<BooleanT>(*value_type, value));
        result = ptr.get();
      },
      [&result, &value](const UIntegerT_Ptr& value_type) {
        auto ptr = move(getValueName<UIntegerT>(*value_type, value));
        result = ptr.get();
      },
      [&result, &value](const IntegerT_Ptr& value_type) {
        auto ptr = move(getValueName<IntegerT>(*value_type, value));
        result = ptr.get();
      },
      [&result, &value](const FloatT_Ptr& value_type) {
        auto ptr = move(getValueName<FloatT>(*value_type, value));
        result = ptr.get();
      },
      // Record Types
      [&result, &value, subindex](const RecordT_Ptr& value_type) {
        auto ptr = move(getValueName(*value_type, subindex, value));
        result = ptr.get();
      },
      // rest of types
      [&](auto) {
        throw logic_error(toString(toDatatype(type)) +
            " does not support named value attribute lookup");
      });
  if (result != nullptr) {
    // TODO: Does this cause seg faults?
    return shared_ptr<NamedAttribute>(result);
  } else {
    throw runtime_error("Given simple value does not have a matching name");
  }
}

Datatype toDatatype(const DataValue& variant) {
  Datatype result;
  match(
      variant,
      [&](const BooleanT_Ptr&) { result = Datatype::Boolean; },
      [&](const UIntegerT_Ptr&) { result = Datatype::UInteger; },
      [&](const IntegerT_Ptr&) { result = Datatype::Integer; },
      [&](const FloatT_Ptr&) { result = Datatype::Float32; },
      [&](const OctetStringT_Ptr&) { result = Datatype::OctetString; },
      [&](const StringT_Ptr&) { result = Datatype::String; },
      [&](const TimeT_Ptr&) { result = Datatype::Time; },
      [&](const TimeSpanT_Ptr&) { result = Datatype::TimeSpan; },
      // array types
      [&](const ArrayT_Ptr<BooleanT_Ptr>&) {
        result = Datatype::Array_Boolean;
      },
      [&](const ArrayT_Ptr<UIntegerT_Ptr>&) {
        result = Datatype::Array_UInteger;
      },
      [&](const ArrayT_Ptr<IntegerT_Ptr>&) {
        result = Datatype::Array_Integer;
      },
      [&](const ArrayT_Ptr<FloatT_Ptr>&) { result = Datatype::Array_Float32; },
      [&](const ArrayT_Ptr<OctetStringT_Ptr>&) {
        result = Datatype::Array_OctetString;
      },
      [&](const ArrayT_Ptr<StringT_Ptr>&) { result = Datatype::Array_String; },
      [&](const ArrayT_Ptr<TimeT_Ptr>&) { result = Datatype::Array_Time; },
      [&](const ArrayT_Ptr<TimeSpanT_Ptr>&) {
        result = Datatype::Array_TimeSpan;
      },
      // record types
      [&](const RecordT_Ptr&) { result = Datatype::Record; });
  return result;
}
} // namespace IODD
