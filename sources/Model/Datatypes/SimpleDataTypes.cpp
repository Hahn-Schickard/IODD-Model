#include "SimpleDataTypes.hpp"

#include <Variant_Visitor/Visitor.hpp>

#include <algorithm>
#include <regex>
#include <unordered_map>

using namespace std;

namespace IODD {

Datatype toDatatype(const SimpleDatatype& variant) {
  return Variant_Visitor::match(
      variant,
      [](const BooleanT_Ptr&) { return Datatype::Boolean; },
      [](const UIntegerT_Ptr&) { return Datatype::UInteger; },
      [](const IntegerT_Ptr&) { return Datatype::Integer; },
      [](const FloatT_Ptr&) { return Datatype::Float32; },
      [](const OctetStringT_Ptr&) { return Datatype::OctetString; },
      [](const StringT_Ptr&) { return Datatype::String; },
      [](const TimeT_Ptr&) { return Datatype::Time; },
      [](const TimeSpanT_Ptr&) { return Datatype::TimeSpan; });
}

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
  return static_cast<uint8_t>(type) < static_cast<uint8_t>(Datatype::Array);
}

bool isComplexData(Datatype type) {
  if (isSimpleData(type)) {
    return false;
  } else {
    return static_cast<uint8_t>(type) <
        static_cast<uint8_t>(Datatype::ProcessDataIn);
  }
}

bool isArray(Datatype type) {
  if (!isComplexData(type)) {
    return false;
  } else {
    return static_cast<uint8_t>(type) < static_cast<uint8_t>(Datatype::Record);
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
  case Datatype::Array: {
    return "ArrayT";
  }
  case Datatype::Record: {
    return "RecordT";
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

SimpleDatatypeValue::SimpleDatatypeValue(SimpleDatatypeValue::Value&& value)
    : value_(move(value)) {}

SimpleDatatypeValue::Value SimpleDatatypeValue::operator()() const {
  return value_;
}

string SimpleDatatypeValue::asString() const {
  return Variant_Visitor::match(
      value_,
      [](bool value) { return value ? "True" : "False"; },
      [](string value) { return value; },
      [](auto value) { return to_string(value); });
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
      {"ARRAYT", Datatype::Array},
      {"RECORDT", Datatype::Record},
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

void expand(SimpleDatatype& lhs, const SimpleDatatype& rhs) {
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
      [&rhs](const OctetStringT_Ptr&) { /* no expansion for OctetStringT */ },
      [&rhs](const StringT_Ptr&) { /* no expansion for StringT */ },
      [&rhs](const TimeT_Ptr&) { /* no expansion for TimeT */ },
      [&rhs](const TimeSpanT_Ptr&) { /* no expansion for TimeSpanT */ });
}
} // namespace IODD
