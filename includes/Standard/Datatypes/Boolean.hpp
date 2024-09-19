#ifndef __IODD_STANDARD_DEFINES_BOOLEAN_T_HPP
#define __IODD_STANDARD_DEFINES_BOOLEAN_T_HPP

#include "Primitives/ValueTypes.hpp"

#include <unordered_set>

namespace IODD {
struct BooleanT {
  using Values = std::unordered_set<SingleValuePtr<bool>>;

  BooleanT() = default;

  BooleanT(std::unordered_set<SingleValuePtr<bool>>&& values);

  void expand(const BooleanT& other);

  NamedAttributePtr getName(bool value) const;

  size_t hash() const noexcept;

  Values values() const;

private:
  Values values_;
};

inline bool operator==(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() > rhs.hash();
}
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_BOOLEAN_T_HPP