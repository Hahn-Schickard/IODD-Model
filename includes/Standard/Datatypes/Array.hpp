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
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_ARRAY_T_HPP