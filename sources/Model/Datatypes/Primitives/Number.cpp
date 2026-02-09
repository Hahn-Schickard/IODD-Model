#include "Number.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

template <typename T>
NumberT<T>::NumberT(NumberT<T>::SingleValues&& values) :
    values_(move(values)) {}

template <typename T>
NumberT<T>::NumberT(NumberT<T>::ValueRanges&& ranges) :
    ranges_(move(ranges)) {}

template <typename T>
NumberT<T>::NumberT(
    NumberT<T>::SingleValues&& values, NumberT<T>::ValueRanges&& ranges) :
    values_(move(values)),
    ranges_(move(ranges)) {}

template <typename T> NamedAttributePtr NumberT<T>::getName(T value) const {
  if (auto it = values_.find(make_shared<SingleValue<T>>(value));
      it != values_.end()) {
    return *it;
  } else {
    for (const auto& range : ranges_) {
      if (range->inRange(value)) {
        return range;
      }
    }
  }
  throw out_of_range(to_string(value) + " value has no assigned named");
}

template <typename T> void NumberT<T>::expand(const NumberT& other) {
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

template <typename T> size_t NumberT<T>::hash() const noexcept {
  size_t result = 0;
  for (const auto& value : values_) {
    result += value->hash();
  }
  for (const auto& value : ranges_) {
    result += value->hash();
  }
  return result;
}

template struct NumberT<uint64_t>;
template struct NumberT<int64_t>;
template struct NumberT<float>;
} // namespace IODD
