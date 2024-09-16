#ifndef __IODD_STANDARD_DEFINES_INTEGER_T_HPP
#define __IODD_STANDARD_DEFINES_INTEGER_T_HPP

#include "Primitives/FixedBitLength.hpp"
#include "Primitives/Number.hpp"

#include <unordered_set>

namespace IODD {
struct IntegerT : public FixedBitLength<2, 64>, public NumberT<int64_t> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  IntegerT() = default;

  IntegerT(uint8_t bits, NumberT<int64_t>&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  IntegerT(NumberT<int64_t>&& values) // used as expansion
      : FixedBitLength(64), NumberT(std::move(values)) {}
};
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_INTEGER_T_HPP