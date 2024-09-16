#include "Menu.hpp"

using namespace std;

namespace IODD {

Condition::Condition(const VariablePtr& variable, uint8_t value)
    : variable_(variable), value_(value) {}

Condition::Condition(
    const VariablePtr& variable, uint8_t subindex, uint8_t value)
    : variable_(variable), subindex_(subindex), value_(value) {}

Condition::Condition(
    const VariablePtr& variable, optional<uint8_t> subindex, uint8_t value)
    : variable_(variable), subindex_(subindex), value_(value) {}

VariablePtr Condition::variable() const { return variable_; }

optional<uint8_t> Condition::subindex() const { return subindex_; }

uint8_t Condition::value() const { return value_; }

Menu::Menu(const string& _id,
    const vector<Ref>& _references,
    const optional<TextID>& _name,
    const optional<Condition> _condition)
    : id(_id), references(_references), name(_name), condition(_condition) {}

size_t Menu::size() const { return references.size(); }

} // namespace IODD
