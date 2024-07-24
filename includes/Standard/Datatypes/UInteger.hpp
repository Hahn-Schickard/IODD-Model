#ifndef __IODD_STANDARD_DEFINES_UINTEGER_T_HPP
#define __IODD_STANDARD_DEFINES_UINTEGER_T_HPP

#include "Primitives/FixedBitLength.hpp"
#include "Primitives/Number.hpp"

#include <unordered_set>

namespace IODD {
struct UIntegerT : public FixedBitLength<2, 64>, public NumberT<uint64_t> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  UIntegerT() = default;

  UIntegerT(uint8_t bits, SingleValues&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  UIntegerT(uint8_t bits, ValueRanges&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  UIntegerT(
      uint8_t bits, SingleValues&& single_values, ValueRanges&& value_ranges)
      : FixedBitLength(bits),
        NumberT(std::move(single_values), std::move(value_ranges)) {}
};
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_UINTEGER_T_HPP