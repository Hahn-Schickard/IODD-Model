#ifndef __IODD_STANDARD_DEFINES_FLOAT_T_HPP
#define __IODD_STANDARD_DEFINES_FLOAT_T_HPP

#include "Primitives/Number.hpp"

#include <unordered_set>

namespace IODD {

struct FloatT : public NumberT<float> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  FloatT() = default;

  FloatT(NumberT<float>&& values) : NumberT(std::move(values)) {}
};

} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_FLOAT_T_HPP