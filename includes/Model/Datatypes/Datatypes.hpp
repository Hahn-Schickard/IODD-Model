#ifndef __IODD_STANDARD_DEFINES_DATA_TYPES_HPP
#define __IODD_STANDARD_DEFINES_DATA_TYPES_HPP

#include "Array.hpp"
#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "OctetString.hpp"
#include "ProcessDataUnion.hpp"
#include "Record.hpp"
#include "String.hpp"
#include "Time.hpp"
#include "Timespan.hpp"
#include "UInteger.hpp"

#include <unordered_map>
#include <variant>

namespace IODD {

using SimpleDatatypeValue = // TimeT and TimeSpanT are stored as strings
    std::variant<bool, uint64_t, int64_t, float, std::string>;

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
  Array_UInteger = 10,
  Array_Integer = 11,
  Array_Float32 = 12,
  Array_Boolean = 13,
  Array_String = 14,
  Array_OctetString = 15,
  Array_Time = 16,
  Array_TimeSpan = 17,
  Record_UInteger = 20,
  Record_Integer = 21,
  Record_Float32 = 22,
  Record_Boolean = 23,
  Record_String = 24,
  Record_OctetString = 25,
  Record_Time = 26,
  Record_TimeSpan = 27,
  ProcessDataIn = 30,
  ProcessDataOut = 31
};

bool isNumericData(Datatype type);

bool isSimpleData(Datatype type);

bool isComplexData(Datatype type);

bool isArray(Datatype type);

bool isRecord(Datatype type);

bool isProcessData(Datatype type);

std::string toString(Datatype type);

Datatype toDatatype(const std::string& value);

Datatype toDatatype(const SimpleDatatype& variant);

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
        RecordT<TimeSpanT>,
        ProcessDataIn,
        ProcessDataOut
>; // clang-format on

using DatatypesMap = std::unordered_map<std::string, DataValue>;
using DatatypesMapPtr = std::shared_ptr<DatatypesMap>;

Datatype toDatatype(const DataValue& variant);

void expand(DataValue& lhs, const DataValue& rhs);

NamedAttributePtr getValueName(const DataValue& type,
    const SimpleDatatypeValue& value,
    std::optional<uint8_t> subindex = std::nullopt);
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_DATA_TYPES_HPP