#include "SimpleDataTypes.hpp"

#include <Variant_Visitor.hpp>

#include <algorithm>
#include <regex>
#include <unordered_map>

using namespace std;

namespace IODD {

Datatype toDatatype(const SimpleDatatype& variant) {
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
      [&](const TimeSpanT_Ptr&) { result = Datatype::TimeSpan; });
  return result;
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

} // namespace IODD