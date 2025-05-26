#ifndef __IODD_STANDARD_DEFINES_NUMBER_T_HPP
#define __IODD_STANDARD_DEFINES_NUMBER_T_HPP

#include "ValueTypes.hpp"

#include <cstdint>
#include <unordered_set>

namespace IODD {

template <typename T> struct NumberT {
  using SingleValues = std::unordered_set<SingleValuePtr<T>>;
  using ValueRanges = std::unordered_set<ValueRangePtr<T>>;

  NumberT() = default;

  NumberT(SingleValues&& values);

  NumberT(ValueRanges&& ranges);

  NumberT(SingleValues&& values, ValueRanges&& ranges);

  NamedAttributePtr getName(T value) const;

  void expand(const NumberT& other);

  size_t hash() const noexcept;

private:
  SingleValues values_;
  ValueRanges ranges_;
};

extern template struct NumberT<uint64_t>;
extern template struct NumberT<int64_t>;
extern template struct NumberT<float>;

template <typename T>
inline bool operator==(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() == rhs.hash();
}

template <typename T>
inline bool operator!=(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() != rhs.hash();
}

template <typename T>
inline bool operator<=(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() <= rhs.hash();
}

template <typename T>
inline bool operator>=(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() >= rhs.hash();
}

template <typename T>
inline bool operator<(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() < rhs.hash();
}

template <typename T>
inline bool operator>(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() > rhs.hash();
}
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_NUMBER_T_HPP