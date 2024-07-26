#ifndef __IODD_STANDARD_DEFINES_DATA_TYPES_HPP
#define __IODD_STANDARD_DEFINES_DATA_TYPES_HPP

#include "Array.hpp"
#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "OctetString.hpp"
#include "Record.hpp"
#include "String.hpp"
#include "Time.hpp"
#include "Timespan.hpp"
#include "UInteger.hpp"

#include "Variant_Visitor.hpp"

#include <unordered_map>
#include <variant>

namespace IODD {

using SimpleDatatype = std::variant<BooleanT,
    UIntegerT,
    IntegerT,
    FloatT,
    OctetStringT,
    StringT,
    TimeT,
    TimeSpanT>;

enum class Datatype : uint8_t {
  UInteger = 0,
  Integer = 1,
  Float32 = 2,
  Boolean = 3,
  String = 4,
  OctetString = 5,
  Time = 6,
  TimeSpan = 7,
  Array = 8,
  Record = 9,
  ProcessDataIn = 10,
  ProcessDataOut = 11
};

inline bool isNumericData(Datatype type) {
  return static_cast<uint8_t>(type) < 3;
}

inline bool isSimpleData(Datatype type) {
  return static_cast<uint8_t>(type) < 8;
}

inline bool isComplexData(Datatype type) {
  auto type_v = static_cast<uint8_t>(type);
  return (type_v > 7 && type_v < 10);
}

inline bool isProcessData(Datatype type) {
  return static_cast<uint8_t>(type) > 9;
}

inline std::string toString(Datatype type) {
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
    throw std::runtime_error("Unhandled Datatype enum value");
  }
  }
}

inline Datatype toDatatype(const std::string& value) {
  const std::unordered_map<std::string, Datatype> data_types = {
      {"BooleanT", Datatype::Boolean},
      {"UIntegerT", Datatype::UInteger},
      {"IntegerT", Datatype::Integer},
      {"Float32T", Datatype::Float32},
      {"StringT", Datatype::String},
      {"OctetStringT", Datatype::OctetString},
      {"TimeT", Datatype::Time},
      {"TimeSpanT", Datatype::TimeSpan},
      {"ArrayT", Datatype::Array},
      {"RecordT", Datatype::Record},
      {"ProcessDataInUnionT", Datatype::ProcessDataIn},
      {"ProcessDataOutUnionT", Datatype::ProcessDataOut}};

  if (auto it = data_types.find(value); it != data_types.end()) {
    return it->second;
  }
  throw std::invalid_argument("String value: " + value +
      " can not be converted into IODD::Datatype enumeration");
}

inline Datatype toDatatype(const SimpleDatatype& variant) {
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

using DataValue = std::variant< // clang-format off
        BooleanT, 
        UIntegerT, 
        IntegerT, 
        FloatT, 
        OctetStringT, 
        StringT,
        TimeT, 
        TimeSpanT, 
        ArrayT<BooleanT>, 
        ArrayT<UIntegerT>, 
        ArrayT<IntegerT>,
        ArrayT<FloatT>, 
        ArrayT<OctetStringT>, 
        ArrayT<StringT>, 
        ArrayT<TimeT>,
        ArrayT<TimeSpanT>, 
        RecordT<BooleanT>, 
        RecordT<UIntegerT>,
        RecordT<IntegerT>, 
        RecordT<FloatT>, 
        RecordT<OctetStringT>,
        RecordT<StringT>, 
        RecordT<TimeT>, 
        RecordT<TimeSpanT>
>; // clang-format on

inline Datatype toDatatype(const DataValue& variant) {
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
      [&](const ArrayT<BooleanT>&) { result = Datatype::Array; },
      [&](const ArrayT<UIntegerT>&) { result = Datatype::Array; },
      [&](const ArrayT<IntegerT>&) { result = Datatype::Array; },
      [&](const ArrayT<FloatT>&) { result = Datatype::Array; },
      [&](const ArrayT<OctetStringT>&) { result = Datatype::Array; },
      [&](const ArrayT<StringT>&) { result = Datatype::Array; },
      [&](const ArrayT<TimeT>&) { result = Datatype::Array; },
      [&](const ArrayT<TimeSpanT>&) { result = Datatype::Array; },
      // record types
      [&](const RecordT<BooleanT>&) { result = Datatype::Record; },
      [&](const RecordT<UIntegerT>&) { result = Datatype::Record; },
      [&](const RecordT<IntegerT>&) { result = Datatype::Record; },
      [&](const RecordT<FloatT>&) { result = Datatype::Record; },
      [&](const RecordT<OctetStringT>&) { result = Datatype::Record; },
      [&](const RecordT<StringT>&) { result = Datatype::Record; },
      [&](const RecordT<TimeT>&) { result = Datatype::Record; },
      [&](const RecordT<TimeSpanT>&) { result = Datatype::Record; });
  return result;
}

inline void expand(DataValue& lhs, const DataValue& rhs) {
  if (lhs.index() != rhs.index()) {
    throw std::logic_error(
        "Expanded data type does not match variable datatype");
  }
  match(
      lhs,
      [&rhs](BooleanT value) { value.expand(std::get<BooleanT>(rhs)); },
      [&rhs](UIntegerT value) { value.expand(std::get<UIntegerT>(rhs)); },
      [&rhs](IntegerT value) { value.expand(std::get<IntegerT>(rhs)); },
      [&rhs](FloatT value) { value.expand(std::get<FloatT>(rhs)); },
      [&rhs](OctetStringT) { /* no expansion for OctetStringT */ },
      [&rhs](StringT) { /* no expansion for StringT */ },
      [&rhs](TimeT) { /* no expansion for TimeT */ },
      [&rhs](TimeSpanT) { /* no expansion for TimeSpanT */ },
      // array types
      [&rhs](ArrayT<BooleanT> value) {
        value.expand(std::get<ArrayT<BooleanT>>(rhs));
      },
      [&rhs](ArrayT<UIntegerT> value) {
        value.expand(std::get<ArrayT<UIntegerT>>(rhs));
      },
      [&rhs](ArrayT<IntegerT> value) {
        value.expand(std::get<ArrayT<IntegerT>>(rhs));
      },
      [&rhs](ArrayT<FloatT> value) {
        value.expand(std::get<ArrayT<FloatT>>(rhs));
      },
      [&rhs](ArrayT<OctetStringT> value) {
        value.expand(std::get<ArrayT<OctetStringT>>(rhs));
      },
      [&rhs](ArrayT<StringT> value) {
        value.expand(std::get<ArrayT<StringT>>(rhs));
      },
      [&rhs](
          ArrayT<TimeT> value) { value.expand(std::get<ArrayT<TimeT>>(rhs)); },
      [&rhs](ArrayT<TimeSpanT> value) {
        value.expand(std::get<ArrayT<TimeSpanT>>(rhs));
      },
      // record types
      [&rhs](RecordT<BooleanT> value) {
        value.expand(std::get<RecordT<BooleanT>>(rhs));
      },
      [&rhs](RecordT<UIntegerT> value) {
        value.expand(std::get<RecordT<UIntegerT>>(rhs));
      },
      [&rhs](RecordT<IntegerT> value) {
        value.expand(std::get<RecordT<IntegerT>>(rhs));
      },
      [&rhs](RecordT<FloatT> value) {
        value.expand(std::get<RecordT<FloatT>>(rhs));
      },
      [&rhs](RecordT<OctetStringT> value) {
        value.expand(std::get<RecordT<OctetStringT>>(rhs));
      },
      [&rhs](RecordT<StringT> value) {
        value.expand(std::get<RecordT<StringT>>(rhs));
      },
      [&rhs](RecordT<TimeT> value) {
        value.expand(std::get<RecordT<TimeT>>(rhs));
      },
      [&rhs](RecordT<TimeSpanT> value) {
        value.expand(std::get<RecordT<TimeSpanT>>(rhs));
      });
}
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_DATA_TYPES_HPP