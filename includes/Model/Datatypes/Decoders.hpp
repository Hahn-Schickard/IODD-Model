#ifndef __IODD_STANDARD_DEFINES_DECODERS_HPP
#define __IODD_STANDARD_DEFINES_DECODERS_HPP

#include "Datatypes.hpp"

#include <stdint.h>
#include <vector>

namespace IODD {

SimpleDatatypeValue decodeValue(
    const std::vector<uint8_t>& bytes, const SimpleDatatype& type);

SimpleDatatypeValue decodeValue(const std::vector<uint8_t>& bytes,
    const DataValue& type,
    std::optional<uint8_t> subindex = std::nullopt);

} // namespace IODD
#endif // __IODD_STANDARD_DEFINES_DECODERS_HPP