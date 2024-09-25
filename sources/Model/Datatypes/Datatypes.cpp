#include "Datatypes.hpp"

#include "Variant_Visitor.hpp"

#include <algorithm>
#include <regex>
#include <unordered_map>

using namespace std;

namespace IODD {

bool isNumericData(Datatype type) {
  return static_cast<uint8_t>(type) < static_cast<uint8_t>(Datatype::Boolean);
}

bool isNumericComplexData(Datatype type) {
  if (!isComplexData(type)) {
    return false;
  } else {
    // NOLINTNEXTLINE(readability-magic-numbers)
    return (static_cast<uint8_t>(type) % 10) <
        static_cast<uint8_t>(Datatype::Boolean);
  }
}

bool isSimpleData(Datatype type) {
  return static_cast<uint8_t>(type) <
      static_cast<uint8_t>(Datatype::Array_UInteger);
}

bool isComplexData(Datatype type) {
  if (isSimpleData(type)) {
    return false;
  } else {
    return static_cast<uint8_t>(type) <
        static_cast<uint8_t>(Datatype::ProcessDataIn);
    ;
  }
}

bool isArray(Datatype type) {
  if (!isComplexData(type)) {
    return false;
  } else {
    return static_cast<uint8_t>(type) <
        static_cast<uint8_t>(Datatype::Record_UInteger);
  }
}

bool isRecord(Datatype type) {
  if (!isComplexData(type)) {
    return false;
  } else {
    return static_cast<uint8_t>(type) <
        static_cast<uint8_t>(Datatype::ProcessDataIn);
  }
}

bool isProcessData(Datatype type) {
  return static_cast<uint8_t>(type) >=
      static_cast<uint8_t>(Datatype::ProcessDataIn);
}

string toString(Datatype type) {
  switch (type) {
  case Datatype::Boolean: {
    return "BooleanT";
  }
  case Datatype::UInteger: {
    return "UIntegerT";
  }
  case Datatype::Integer: {
    return "IntegerT";
  }
  case Datatype::Float32: {
    return "Float32T";
  }
  case Datatype::String: {
    return "StringT";
  }
  case Datatype::OctetString: {
    return "OctetStringT";
  }
  case Datatype::Time: {
    return "TimeT";
  }
  case Datatype::TimeSpan: {
    return "TimeSpanT";
  }
  case Datatype::Array_Boolean: {
    return "BooleanT ArrayT";
  }
  case Datatype::Array_UInteger: {
    return "UIntegerT ArrayT";
  }
  case Datatype::Array_Integer: {
    return "IntegerT ArrayT";
  }
  case Datatype::Array_Float32: {
    return "Float32T ArrayT";
  }
  case Datatype::Array_String: {
    return "StringT ArrayT";
  }
  case Datatype::Array_OctetString: {
    return "OctetStringT ArrayT";
  }
  case Datatype::Array_Time: {
    return "TimeT ArrayT";
  }
  case Datatype::Array_TimeSpan: {
    return "TimeSpanT ArrayT";
  }
  case Datatype::Record_Boolean: {
    return "BooleanT RecordT";
  }
  case Datatype::Record_UInteger: {
    return "UIntegerT RecordT";
  }
  case Datatype::Record_Integer: {
    return "IntegerT RecordT";
  }
  case Datatype::Record_Float32: {
    return "Float32T RecordT";
  }
  case Datatype::Record_String: {
    return "StringT RecordT";
  }
  case Datatype::Record_OctetString: {
    return "OctetStringT RecordT";
  }
  case Datatype::Record_Time: {
    return "TimeT RecordT";
  }
  case Datatype::Record_TimeSpan: {
    return "TimeSpanT RecordT";
  }
  case Datatype::ProcessDataIn: {
    return "ProcessDataInT";
  }
  case Datatype::ProcessDataOut: {
    return "ProcessDataOutT";
  }
  default: {
    throw runtime_error("Unhandled Datatype enum value");
  }
  }
}

Datatype toDatatype(const string& value) {
  const unordered_map<string, Datatype> data_types = {
      {"BOOLEANT", Datatype::Boolean},
      {"UINTEGERT", Datatype::UInteger},
      {"INTEGERT", Datatype::Integer},
      {"FLOAT32T", Datatype::Float32},
      {"STRINGT", Datatype::String},
      {"OCTETSTRINGT", Datatype::OctetString},
      {"TIMET", Datatype::Time},
      {"TIMESPANT", Datatype::TimeSpan},
      {"BOOLEANT_ARRAYT", Datatype::Array_Boolean},
      {"UINTEGERT_ARRAYT", Datatype::Array_UInteger},
      {"INTEGERT_ARRAYT", Datatype::Array_Integer},
      {"FLOAT32T_ARRAYT", Datatype::Array_Float32},
      {"STRINGT_ARRAYT", Datatype::Array_String},
      {"OCTETSTRINGT_ARRAYT", Datatype::Array_OctetString},
      {"TIMET_ARRAYT", Datatype::Array_Time},
      {"TIMESPANT_ARRAYT", Datatype::Array_TimeSpan},
      {"BOOLEANT_RECORDT", Datatype::Record_Boolean},
      {"UINTEGERT_RECORDT", Datatype::Record_UInteger},
      {"INTEGERT_RECORDT", Datatype::Record_Integer},
      {"FLOAT32T_RECORDT", Datatype::Record_Float32},
      {"STRINGT_RECORDT", Datatype::Record_String},
      {"OCTETSTRINGT_RECORDT", Datatype::Record_OctetString},
      {"TIMET_RECORDT", Datatype::Record_Time},
      {"TIMESPANT_RECORDT", Datatype::Record_TimeSpan},
      {"PROCESSDATAINUNIONT", Datatype::ProcessDataIn},
      {"PROCESSDATAOUTUNIONT", Datatype::ProcessDataOut}};

  auto sanitized = regex_replace(value, regex("^\\s+"), "");
  sanitized = regex_replace(sanitized, regex("\\s+$"), "");
  sanitized = regex_replace(sanitized, regex("\\s+"), "_");
  transform(sanitized.begin(), sanitized.end(), sanitized.begin(), ::toupper);
  if (auto it = data_types.find(sanitized); it != data_types.end()) {
    return it->second;
  }
  throw invalid_argument("String value: " + value +
      " can not be converted into IODD::Datatype enumeration");
}

Datatype toDatatype(const SimpleDatatype& variant) {
  Datatype result;
  match(
      variant,
      [&](const BooleanT&) { result = Datatype::Boolean; },
      [&](const UIntegerT&) { result = Datatype::UInteger; },
      [&](const IntegerT&) { result = Datatype::Integer; },
      [&](const FloatT&) { result = Datatype::Float32; },
      [&](const OctetStringT&) { result = Datatype::OctetString; },
      [&](const StringT&) { result = Datatype::String; },
      [&](const TimeT&) { result = Datatype::Time; },
      [&](const TimeSpanT&) { result = Datatype::TimeSpan; });
  return result;
}

Datatype toDatatype(const DataValue& variant) {
  Datatype result;
  match(
      variant,
      [&](const BooleanT&) { result = Datatype::Boolean; },
      [&](const UIntegerT&) { result = Datatype::UInteger; },
      [&](const IntegerT&) { result = Datatype::Integer; },
      [&](const FloatT&) { result = Datatype::Float32; },
      [&](const OctetStringT&) { result = Datatype::OctetString; },
      [&](const StringT&) { result = Datatype::String; },
      [&](const TimeT&) { result = Datatype::Time; },
      [&](const TimeSpanT&) { result = Datatype::TimeSpan; },
      // array types
      [&](const ArrayT<BooleanT>&) { result = Datatype::Array_Boolean; },
      [&](const ArrayT<UIntegerT>&) { result = Datatype::Array_UInteger; },
      [&](const ArrayT<IntegerT>&) { result = Datatype::Array_Integer; },
      [&](const ArrayT<FloatT>&) { result = Datatype::Array_Float32; },
      [&](const ArrayT<OctetStringT>&) {
        result = Datatype::Array_OctetString;
      },
      [&](const ArrayT<StringT>&) { result = Datatype::Array_String; },
      [&](const ArrayT<TimeT>&) { result = Datatype::Array_Time; },
      [&](const ArrayT<TimeSpanT>&) { result = Datatype::Array_TimeSpan; },
      // record types
      [&](const RecordT<BooleanT>&) { result = Datatype::Record_Boolean; },
      [&](const RecordT<UIntegerT>&) { result = Datatype::Record_UInteger; },
      [&](const RecordT<IntegerT>&) { result = Datatype::Record_Integer; },
      [&](const RecordT<FloatT>&) { result = Datatype::Record_Float32; },
      [&](const RecordT<OctetStringT>&) {
        result = Datatype::Record_OctetString;
      },
      [&](const RecordT<StringT>&) { result = Datatype::Record_String; },
      [&](const RecordT<TimeT>&) { result = Datatype::Record_Time; },
      [&](const RecordT<TimeSpanT>&) { result = Datatype::Record_TimeSpan; },
      [&](const ProcessDataIn&) { result = Datatype::ProcessDataIn; },
      [&](const ProcessDataOut&) { result = Datatype::ProcessDataOut; });
  return result;
}

void expand(DataValue& lhs, const DataValue& rhs) {
  if (lhs.index() != rhs.index()) {
    throw logic_error("Expanded data type does not match variable datatype");
  }
  match(
      lhs,
      [&rhs](BooleanT value) { value.expand(get<BooleanT>(rhs)); },
      [&rhs](UIntegerT value) { value.expand(get<UIntegerT>(rhs)); },
      [&rhs](IntegerT value) { value.expand(get<IntegerT>(rhs)); },
      [&rhs](FloatT value) { value.expand(get<FloatT>(rhs)); },
      [&rhs](OctetStringT) { /* no expansion for OctetStringT */ },
      [&rhs](StringT) { /* no expansion for StringT */ },
      [&rhs](TimeT) { /* no expansion for TimeT */ },
      [&rhs](TimeSpanT) { /* no expansion for TimeSpanT */ },
      // array types
      [&rhs](
          ArrayT<BooleanT> value) { value.expand(get<ArrayT<BooleanT>>(rhs)); },
      [&rhs](ArrayT<UIntegerT> value) {
        value.expand(get<ArrayT<UIntegerT>>(rhs));
      },
      [&rhs](
          ArrayT<IntegerT> value) { value.expand(get<ArrayT<IntegerT>>(rhs)); },
      [&rhs](ArrayT<FloatT> value) { value.expand(get<ArrayT<FloatT>>(rhs)); },
      [&rhs](ArrayT<OctetStringT> value) {
        value.expand(get<ArrayT<OctetStringT>>(rhs));
      },
      [&rhs](
          ArrayT<StringT> value) { value.expand(get<ArrayT<StringT>>(rhs)); },
      [&rhs](ArrayT<TimeT> value) { value.expand(get<ArrayT<TimeT>>(rhs)); },
      [&rhs](ArrayT<TimeSpanT> value) {
        value.expand(get<ArrayT<TimeSpanT>>(rhs));
      },
      // record types
      [&rhs](RecordT<BooleanT> value) {
        value.expand(get<RecordT<BooleanT>>(rhs));
      },
      [&rhs](RecordT<UIntegerT> value) {
        value.expand(get<RecordT<UIntegerT>>(rhs));
      },
      [&rhs](RecordT<IntegerT> value) {
        value.expand(get<RecordT<IntegerT>>(rhs));
      },
      [&rhs](
          RecordT<FloatT> value) { value.expand(get<RecordT<FloatT>>(rhs)); },
      [&rhs](RecordT<OctetStringT> value) {
        value.expand(get<RecordT<OctetStringT>>(rhs));
      },
      [&rhs](
          RecordT<StringT> value) { value.expand(get<RecordT<StringT>>(rhs)); },
      [&rhs](RecordT<TimeT> value) { value.expand(get<RecordT<TimeT>>(rhs)); },
      [&rhs](RecordT<TimeSpanT> value) {
        value.expand(get<RecordT<TimeSpanT>>(rhs));
      },
      [&rhs](const ProcessDataIn&) { /* no expansion for ProcessDataIn*/ },
      [&rhs](const ProcessDataOut&) { /* no expansion for ProcessDataOut*/ });
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
NamedAttributePtr getValueName(
    const T& value_type, const SimpleDatatypeValue& value) {
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

template <typename T>
NamedAttributePtr getValueName(const RecordT<T>& record,
    const optional<uint8_t>& subindex,
    const SimpleDatatypeValue& value) {
  if (record.subindexAccess()) {
    if (subindex.has_value()) {
      auto item = record.item(subindex.value());
      return getValueName<T>(item.value(), value);
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
      [&result, &value](const BooleanT& value_type) {
        auto ptr = move(getValueName<BooleanT>(value_type, value));
        result = ptr.get();
      },
      [&result, &value](const UIntegerT& value_type) {
        auto ptr = move(getValueName<UIntegerT>(value_type, value));
        result = ptr.get();
      },
      [&result, &value](const IntegerT& value_type) {
        auto ptr = move(getValueName<IntegerT>(value_type, value));
        result = ptr.get();
      },
      [&result, &value](const FloatT& value_type) {
        auto ptr = move(getValueName<FloatT>(value_type, value));
        result = ptr.get();
      },
      // Record Types
      [&result, &value, subindex](const RecordT<BooleanT>& value_type) {
        auto ptr = move(getValueName<BooleanT>(value_type, subindex, value));
        result = ptr.get();
      },
      [&result, &value, subindex](const RecordT<UIntegerT>& value_type) {
        auto ptr = move(getValueName<UIntegerT>(value_type, subindex, value));
        result = ptr.get();
      },
      [&result, &value, subindex](const RecordT<IntegerT>& value_type) {
        auto ptr = move(getValueName<IntegerT>(value_type, subindex, value));
        result = ptr.get();
      },
      [&result, &value, subindex](const RecordT<FloatT>& value_type) {
        auto ptr = move(getValueName<FloatT>(value_type, subindex, value));
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

} // namespace IODD
