#ifndef __IODD_STANDARD_DEFINES_FLOAT_T_HPP
#define __IODD_STANDARD_DEFINES_FLOAT_T_HPP

#include "Primitives/Number.hpp"

#include <unordered_set>

namespace IODD {

struct FloatT : public NumberT<float> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  FloatT() = default;

  explicit FloatT(NumberT<float>&& values) : NumberT(std::move(values)) {}

  size_t length() const { return 4; }
};

using FloatT_Ptr = std::shared_ptr<FloatT>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_FLOAT_T_HPP