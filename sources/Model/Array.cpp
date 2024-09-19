#include "Datatypes/Array.hpp"

using namespace std;

namespace IODD {

template <typename T>
ArrayT<T>::ArrayT(size_t count, vector<T>&& values)
    : ArrayT(count, false, move(values)) {}

template <typename T>
ArrayT<T>::ArrayT(size_t count, bool subindex_access, vector<T>&& values)
    : ComplexDataTypeT<T, IsSimpleDatatype<T>>(subindex_access), count_(count),
      values_(move(values)) {}

template <typename T> void ArrayT<T>::expand(const ArrayT& other) {
  values_.insert(values_.end(), other.values_.begin(), other.values_.end());
}

template <typename T> size_t ArrayT<T>::hash() const noexcept {
  size_t result = 0;
  for (const auto& value : values_) {
    result += value.hash();
  }
  return result;
}

template <typename T> size_t ArrayT<T>::count() const { return count_; }

template <typename T> vector<T> ArrayT<T>::values() const { return values_; }

template struct ArrayT<BooleanT>;
template struct ArrayT<UIntegerT>;
template struct ArrayT<IntegerT>;
template struct ArrayT<FloatT>;
template struct ArrayT<StringT>;
template struct ArrayT<OctetStringT>;
template struct ArrayT<TimeT>;
template struct ArrayT<TimeSpanT>;
} // namespace IODD
