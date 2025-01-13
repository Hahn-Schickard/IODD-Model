#ifndef __IODD_STANDARD_DEFINES_ARRAY_T_HPP
#define __IODD_STANDARD_DEFINES_ARRAY_T_HPP

#include "ComplexDatatype.hpp"
#include "SimpleDataTypes.hpp"

#include <vector>

namespace IODD {

template <class T> struct ArrayT : public ComplexDataTypeT {
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

template <class T> using ArrayT_Ptr = std::shared_ptr<ArrayT<T>>;

extern template struct ArrayT<BooleanT_Ptr>;
extern template struct ArrayT<UIntegerT_Ptr>;
extern template struct ArrayT<IntegerT_Ptr>;
extern template struct ArrayT<FloatT_Ptr>;
extern template struct ArrayT<StringT_Ptr>;
extern template struct ArrayT<OctetStringT_Ptr>;
extern template struct ArrayT<TimeT_Ptr>;
extern template struct ArrayT<TimeSpanT_Ptr>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_ARRAY_T_HPP