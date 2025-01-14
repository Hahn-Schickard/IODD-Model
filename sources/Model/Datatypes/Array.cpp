#include "Array.hpp"

using namespace std;

namespace IODD {

ArrayT::ArrayT(Datatype type, size_t count, Values&& values)
    : ArrayT(false, type, count, move(values)) {}

ArrayT::ArrayT(
    bool subindex_access, Datatype type, size_t count, Values&& values)
    : ComplexDataTypeT(subindex_access), type_(type), count_(count),
      values_(move(values)) {}

void ArrayT::expand(const ArrayT& other) {
  values_.insert(values_.end(), other.values_.begin(), other.values_.end());
}

size_t ArrayT::hash() const noexcept {
  size_t result = 0;
  for (const auto& value : values_) {
    result += IODD::hash(value);
  }
  return result;
}

size_t ArrayT::count() const { return count_; }

ArrayT::Values ArrayT::values() const { return values_; }

Datatype ArrayT::type() const { return type_; }
} // namespace IODD
