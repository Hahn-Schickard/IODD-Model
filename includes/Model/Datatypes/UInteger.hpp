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

  UIntegerT(uint8_t bits, NumberT<uint64_t>&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  UIntegerT(NumberT<uint64_t>&& values) // used as expansion
      : FixedBitLength(64), NumberT(std::move(values)) {}
};

using UIntegerT_Ptr = std::shared_ptr<UIntegerT>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_UINTEGER_T_HPP