#ifndef __IODD_STANDARD_DEFINES_BOOLEAN_T_HPP
#define __IODD_STANDARD_DEFINES_BOOLEAN_T_HPP

#include "Primitives/ValueTypes.hpp"

#include <unordered_set>

namespace IODD {
struct BooleanT {
  using Values = std::unordered_set<SingleValuePtr<bool>>;

  BooleanT() = default;

  BooleanT(std::unordered_set<SingleValuePtr<bool>>&& values)
      : values_(std::move(values)) {}

  void expand(const BooleanT& other) {
    for (const auto& value : other.values_) {
      if (values_.find(value) == values_.end()) {
        values_.insert(value);
      }
    }
  }

  NamedAttributePtr getName(bool value) const {
    if (auto it = values_.find(std::make_shared<SingleValue<bool>>(value));
        it != values_.end()) {
      return *it;
    }
    throw std::out_of_range(
        std::string(value ? "True" : "False") + " value has no assigned named");
  }

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : values_) {
      result += value->hash();
    }
    return result;
  }

  Values values() const { return values_; }

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