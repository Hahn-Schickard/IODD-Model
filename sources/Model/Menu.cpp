#include "Menu.hpp"

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

Menu::Menu(const string& id,
    const vector<Ref>& references,
    const optional<TextID>& name,
    const optional<Condition>& condition)
    : id_(id), references_(references), name_(name), condition_(condition) {
  if (id_.empty()) {
    throw invalid_argument("Menu ID can not be empty");
  }
  if (references_.empty()) {
    throw invalid_argument("Menu References can not be empty");
  }
}

size_t Menu::size() const { return references_.size(); }

string Menu::id() const { return id_; }

Menu::Refs Menu::references() const { return references_; }

optional<TextID> Menu::name() const { return name_; }

optional<Condition> Menu::condition() const { return condition_; }

} // namespace IODD
