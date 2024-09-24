#include "Condition.hpp"

using namespace std;

namespace IODD {

Condition::Condition(const VariablePtr& variable, uint8_t value)
    : variable_(variable), value_(value) {}

Condition::Condition(
    const VariablePtr& variable, uint8_t subindex, uint8_t value)
    : variable_(variable), subindex_(subindex), value_(value) {}

Condition::Condition(const VariablePtr& variable,
    const optional<uint8_t>& subindex,
    uint8_t value)
    : variable_(variable), subindex_(subindex), value_(value) {}

VariablePtr Condition::variable() const { return variable_; }

optional<uint8_t> Condition::subindex() const { return subindex_; }

uint8_t Condition::value() const { return value_; }
} // namespace IODD
