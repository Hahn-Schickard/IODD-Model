#ifndef __IODD_STANDARD_DEFINES_VALUE_TYPES_HPP
#define __IODD_STANDARD_DEFINES_VALUE_TYPES_HPP

#include "NamedAttribute.hpp"
#include <cstdint>

namespace IODD {

template <typename T> struct SingleValue : public NamedAttribute {
  SingleValue() = default;

  SingleValue(T value) : value_(value) {}

  SingleValue(T value, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), value_(value) {}

  size_t hash() const noexcept { return std::hash<T>{}(value_); }

  T value() const { return value_; }

private:
  T value_;
};

template <typename T> using SingleValuePtr = std::shared_ptr<SingleValue<T>>;

template <typename T> struct ValueRange : public NamedAttribute {
  ValueRange() = default;

  ValueRange(T lower, T upper) : lower_(lower), upper_(upper) {
    if (upper_ <= lower_) {
      throw std::invalid_argument(
          "Upper bound must be larger than lower bound");
    }
  }

  ValueRange(T lower, T upper, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), lower_(lower), upper_(upper) {}

  bool inRange(T value) const noexcept {
    return (value > lower_) && (value < upper_);
  }

  size_t hash() const noexcept {
    return std::hash<T>{}(lower_) + std::hash<T>{}(upper_);
  }

  T lower() const { return lower_; }

  T upper() const { return upper_; }

private:
  T lower_;
  T upper_;
};

template <typename T> using ValueRangePtr = std::shared_ptr<ValueRange<T>>;

template <typename T>
inline bool operator==(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() == rhs.value();
}

template <typename T>
inline bool operator!=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() != rhs.value();
}

template <typename T>
inline bool operator<=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() <= rhs.value();
}

template <typename T>
inline bool operator>=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() >= rhs.value();
}

template <typename T>
inline bool operator<(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() < rhs.value();
}

template <typename T>
inline bool operator>(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() > rhs.value();
}

template <typename T>
inline bool operator==(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() == rhs.lower()) && (lhs.upper() == rhs.upper());
}

template <typename T>
inline bool operator!=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() != rhs.lower()) && (lhs.upper() != rhs.upper());
}

template <typename T>
inline bool operator<=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() <= rhs.lower()) && (lhs.upper() <= rhs.upper());
}

template <typename T>
inline bool operator>=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() >= rhs.lower()) && (lhs.upper() >= rhs.upper());
}

template <typename T>
inline bool operator<(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() < rhs.lower()) && (lhs.upper() < rhs.upper());
}

template <typename T>
inline bool operator>(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() > rhs.lower()) && (lhs.upper() > rhs.upper());
}

} // namespace IODD

template <typename T> struct std::hash<IODD::SingleValue<T>> {
  std::size_t operator()(const IODD::SingleValue<T>& object) const noexcept {
    return object.hash();
  }
};

template <typename T> struct std::hash<IODD::ValueRange<T>> {
  std::size_t operator()(const IODD::ValueRange<T>& object) const noexcept {
    return object.hash();
  }
};

#endif //__IODD_STANDARD_DEFINES_VALUE_TYPES_HPP