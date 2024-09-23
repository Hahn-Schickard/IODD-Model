#include "Boolean.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

BooleanT::BooleanT(unordered_set<SingleValuePtr<bool>>&& values)
    : values_(move(values)) {}

void BooleanT::expand(const BooleanT& other) {
  for (const auto& value : other.values_) {
    if (values_.find(value) == values_.end()) {
      values_.insert(value);
    }
  }
}

NamedAttributePtr BooleanT::getName(bool value) const {
  if (const auto& it = values_.find(make_shared<SingleValue<bool>>(value));
      it != values_.end()) {
    return *it;
  }
  throw out_of_range(
      string(value ? "True" : "False") + " value has no assigned named");
}

size_t BooleanT::hash() const noexcept {
  size_t result = 0;
  for (const auto& value : values_) {
    result += value->hash();
  }
  return result;
}

BooleanT::Values BooleanT::values() const { return values_; }

} // namespace IODD
