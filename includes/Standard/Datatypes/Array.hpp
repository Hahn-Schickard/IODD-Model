#ifndef __IODD_STANDARD_DEFINES_ARRAY_T_HPP
#define __IODD_STANDARD_DEFINES_ARRAY_T_HPP

#include "ComplexDatatype.hpp"

#include <vector>

namespace IODD {

template <typename T>
struct ArrayT : public ComplexDataTypeT<T, IsSimpleDatatype<T>> {
  ArrayT() = default;

  ArrayT(size_t count, std::vector<T>&& values)
      : ArrayT(count, false, std::move(values)) {}

  ArrayT(size_t count, bool subindex_access, std::vector<T>&& values)
      : ComplexDataTypeT<T, IsSimpleDatatype<T>>(subindex_access),
        count_(count), values_(std::move(values)) {}

  void expand(const ArrayT& other) {
    values_.insert(values_.end(), other.values_.begin(), other.values_.end());
  }

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : values_) {
      result += value.hash();
    }
    return result;
  }

  size_t count() const { return count_; }

  std::vector<T> values() const { return values_; }

private:
  size_t count_;
  std::vector<T> values_;
};
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_ARRAY_T_HPP