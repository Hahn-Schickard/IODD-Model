#ifndef __IODD_STANDARD_DEFINES_NUMBER_T_HPP
#define __IODD_STANDARD_DEFINES_NUMBER_T_HPP

#include "ValueTypes.hpp"

namespace IODD {

template <typename T> struct NumberT {
  using SingleValues = std::unordered_set<SingleValuePtr<T>>;
  using ValueRanges = std::unordered_set<ValueRangePtr<T>>;

  NumberT() = default;

  NumberT(SingleValues&& values) : values_(std::move(values)) {}

  NumberT(ValueRanges&& ranges) : ranges_(std::move(ranges)) {}

  NumberT(SingleValues&& values, ValueRanges&& ranges)
      : values_(std::move(values)), ranges_(std::move(ranges)) {}

  NamedAttributePtr getName(T value) {
    if (auto it = values_.find(std::make_shared<SingleValue<T>>(value));
        it != values_.end()) {
      return *it;
    } else {
      for (const auto& range : ranges_) {
        if (range->inRange(value)) {
          return range;
        }
      }
    }
    throw std::out_of_range(
        std::to_string(value) + " value has no assigned named");
  }

  void expand(const NumberT& other) {
    for (const auto& value : other.values_) {
      if (values_.find(value) == values_.end()) {
        values_.insert(value);
      }
    }
    for (const auto& range : other.ranges_) {
      if (ranges_.find(range) == ranges_.end()) {
        ranges_.insert(range);
      }
    }
  }

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : values_) {
      result += value->hash();
    }
    for (const auto& value : ranges_) {
      result += value->hash();
    }
    return result;
  }

private:
  SingleValues values_;
  ValueRanges ranges_;
};

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