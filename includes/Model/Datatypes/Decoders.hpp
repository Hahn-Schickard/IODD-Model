#ifndef __IODD_STANDARD_DEFINES_DECODERS_HPP
#define __IODD_STANDARD_DEFINES_DECODERS_HPP

#include "Datatypes.hpp"

#include <stdint.h>
#include <vector>

namespace IODD {

bool decodeValue(const std::vector<uint8_t>& bytes, const BooleanT_Ptr& type);

uint64_t decodeValue(
    const std::vector<uint8_t>& bytes, const UIntegerT_Ptr& type);

float decodeValue(const std::vector<uint8_t>& bytes, const FloatT_Ptr& type);

std::string decodeValue(
    const std::vector<uint8_t>& bytes, const OctetStringT_Ptr& type);

std::string decodeValue(
    const std::vector<uint8_t>& bytes, const StringT_Ptr& type);

std::string decodeValue(
    const std::vector<uint8_t>& bytes, const TimeT_Ptr& type);

std::string decodeValue(
    const std::vector<uint8_t>& bytes, const TimeSpanT_Ptr& type);

SimpleDatatypeValue decodeValue(
    const std::vector<uint8_t>& bytes, const SimpleDatatype& type);

SimpleDatatypeValue decodeValue(const std::vector<uint8_t>& bytes,
    const ArrayT_Ptr& type,
    uint8_t subindex);

SimpleDatatypeValue decodeValue(const std::vector<uint8_t>& bytes,
    const RecordT_Ptr& type,
    uint8_t subindex);

} // namespace IODD
#endif // __IODD_STANDARD_DEFINES_DECODERS_HPP