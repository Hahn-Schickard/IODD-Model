#ifndef __IODD_STANDARD_DEFINES_DATA_TYPES_HPP
#define __IODD_STANDARD_DEFINES_DATA_TYPES_HPP

#include "Array.hpp"
#include "Record.hpp"
#include "SimpleDataTypes.hpp"

#include <unordered_map>
#include <variant>

namespace IODD {

using DataValue = std::variant< // clang-format off
        BooleanT_Ptr, 
        UIntegerT_Ptr, 
        IntegerT_Ptr, 
        FloatT_Ptr, 
        OctetStringT_Ptr, 
        StringT_Ptr,
        TimeT_Ptr, 
        TimeSpanT_Ptr, 
        ArrayT_Ptr,
        RecordT_Ptr
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