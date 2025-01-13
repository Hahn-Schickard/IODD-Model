#include "Array.hpp"

using namespace std;

namespace IODD {

template <class T>
ArrayT<T>::ArrayT(size_t count, vector<T>&& values)
    : ArrayT(count, false, move(values)) {}

template <class T>
ArrayT<T>::ArrayT(size_t count, bool subindex_access, vector<T>&& values)
    : ComplexDataTypeT(subindex_access), count_(count), values_(move(values)) {}

template <class T> void ArrayT<T>::expand(const ArrayT& other) {
  values_.insert(values_.end(), other.values_.begin(), other.values_.end());
}

template <class T> size_t ArrayT<T>::hash() const noexcept {
  size_t result = 0;
  for (const auto& value : values_) {
    result += value->hash();
  }
  return result;
}

template <class T> size_t ArrayT<T>::count() const { return count_; }

template <class T> vector<T> ArrayT<T>::values() const { return values_; }

template struct ArrayT<BooleanT_Ptr>;
template struct ArrayT<UIntegerT_Ptr>;
template struct ArrayT<IntegerT_Ptr>;
template struct ArrayT<FloatT_Ptr>;
template struct ArrayT<StringT_Ptr>;
template struct ArrayT<OctetStringT_Ptr>;
template struct ArrayT<TimeT_Ptr>;
template struct ArrayT<TimeSpanT_Ptr>;
} // namespace IODD
