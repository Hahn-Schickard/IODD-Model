#ifndef __IODD_STANDARD_DEFINES_ARRAY_T_HPP
#define __IODD_STANDARD_DEFINES_ARRAY_T_HPP

#include "ComplexDatatype.hpp"

#include <vector>

namespace IODD {

template <typename T>
struct ArrayT : public ComplexDataTypeT<T, IsSimpleDatatype<T>> {
  ArrayT() = default;

  ArrayT(size_t count, std::vector<T>&& values);

  ArrayT(size_t count, bool subindex_access, std::vector<T>&& values);

  void expand(const ArrayT& other);

  size_t hash() const noexcept;

  size_t count() const;

  std::vector<T> values() const;

private:
  size_t count_;
  std::vector<T> values_;
};

template struct ArrayT<BooleanT>;
template struct ArrayT<UIntegerT>;
template struct ArrayT<IntegerT>;
template struct ArrayT<FloatT>;
template struct ArrayT<StringT>;
template struct ArrayT<OctetStringT>;
template struct ArrayT<TimeT>;
template struct ArrayT<TimeSpanT>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_ARRAY_T_HPP