#ifndef __IODD_STANDARD_DEFINES_SIMPLE_DATA_TYPES_HPP
#define __IODD_STANDARD_DEFINES_SIMPLE_DATA_TYPES_HPP

#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "OctetString.hpp"
#include "String.hpp"
#include "Time.hpp"
#include "Timespan.hpp"
#include "UInteger.hpp"

#include <variant>

namespace IODD {

enum class Datatype : uint8_t {
  UInteger = 0,
  Integer = 1,
  Float32 = 2,
  Boolean = 3,
  String = 4,
  OctetString = 5,
  Time = 6,
  TimeSpan = 7,
  Array_UInteger = 10,
  Array_Integer = 11,
  Array_Float32 = 12,
  Array_Boolean = 13,
  Array_String = 14,
  Array_OctetString = 15,
  Array_Time = 16,
  Array_TimeSpan = 17,
  Record = 20,
  ProcessDataIn = 30,
  ProcessDataOut = 31
};

bool isNumericData(Datatype type);

bool isSimpleData(Datatype type);

bool isComplexData(Datatype type);

bool isNumericComplexData(Datatype type);

bool isArray(Datatype type);

bool isRecord(Datatype type);

bool isProcessData(Datatype type);

std::string toString(Datatype type);

using SimpleDatatypeValue = // TimeT and TimeSpanT are stored as strings
    std::variant<bool, uint64_t, int64_t, float, std::string>;

using SimpleDatatype = std::variant<BooleanT_Ptr,
    UIntegerT_Ptr,
    IntegerT_Ptr,
    FloatT_Ptr,
    OctetStringT_Ptr,
    StringT_Ptr,
    TimeT_Ptr,
    TimeSpanT_Ptr>;

Datatype toDatatype(const SimpleDatatype& variant);

Datatype toDatatype(const std::string& value);

} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_SIMPLE_DATA_TYPES_HPP