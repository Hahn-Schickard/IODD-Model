#include "ValueTypes.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

template <typename T> SingleValue<T>::SingleValue(T value) : value_(value) {}

template <typename T>
SingleValue<T>::SingleValue(T value, optional<TextID>&& name)
    : NamedAttribute(move(name)), value_(value) {}

template <typename T> size_t SingleValue<T>::hash() const noexcept {
  return std::hash<T>{}(value_);
}

template <typename T> T SingleValue<T>::value() const { return value_; }

template <typename T>
ValueRange<T>::ValueRange(T lower, T upper) : lower_(lower), upper_(upper) {
  if (upper_ <= lower_) {
    throw invalid_argument("Upper bound must be larger than lower bound");
  }
}

template <typename T>
ValueRange<T>::ValueRange(T lower, T upper, optional<TextID>&& name)
    : NamedAttribute(move(name)), lower_(lower), upper_(upper) {}

template <typename T> bool ValueRange<T>::inRange(T value) const noexcept {
  return (value > lower_) && (value < upper_);
}

template <typename T> size_t ValueRange<T>::hash() const noexcept {
  return std::hash<T>{}(lower_) + std::hash<T>{}(upper_);
}

template <typename T> T ValueRange<T>::lower() const { return lower_; }

template <typename T> T ValueRange<T>::upper() const { return upper_; }

template struct SingleValue<bool>;
template struct SingleValue<uint64_t>;
template struct SingleValue<int64_t>;
template struct SingleValue<float>;
template struct ValueRange<uint64_t>;
template struct ValueRange<int64_t>;
template struct ValueRange<float>;

} // namespace IODD
