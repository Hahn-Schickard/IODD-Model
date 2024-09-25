#include "Variable.hpp"

#include "Variant_Visitor.hpp"

using namespace std;

namespace IODD {

Variable::Variable(size_t index,
    TextID&& name,
    AccessRights access,
    DataValue&& value,
    optional<TextID>&& desc,
    optional<SimpleDatatypeValue> default_value,
    bool dynamic,
    bool modifies_others,
    bool excluded)
    : index_(index), name_(move(name)), access_(access), value_(move(value)),
      desc_(move(desc)), default_(move(default_value)), dynamic_(dynamic),
      modifies_others_(modifies_others), excluded_(excluded) {}

Variable::Variable(const Variable& other,
    optional<SimpleDatatypeValue> default_value,
    optional<bool> excluded,
    optional<DataValue> value)
    : index_(other.index_), name_(other.name_), access_(other.access_),
      value_(other.value_), desc_(other.desc_),
      default_((default_value ? default_value.value() : other.default_)),
      dynamic_(other.dynamic_), modifies_others_(other.modifies_others_),
      excluded_((excluded ? excluded.value() : other.excluded_)) {
  if (value.has_value()) {
    expand(value_, *value);
  }
}

size_t Variable::index() const { return index_; }

TextID Variable::name() const { return name_; }

AccessRights Variable::access() const { return access_; }

DataValue Variable::value() const { return value_; }

NamedAttributePtr Variable::valueName(
    const SimpleDatatypeValue& value, optional<uint8_t> subindex) const {
  return getValueName(value_, value, subindex);
}

Datatype Variable::type() const { return toDatatype(value_); }

SimpleDatatypeValue Variable::defaultValue() const {
  if (default_.has_value()) {
    return default_.value();
  } else {
    throw runtime_error(name_.id() + " Variable has no default value");
  }
}

optional<TextID> Variable::description() const { return desc_; }

bool Variable::dynamic() const { return dynamic_; }

bool Variable::modifiesOthers() const { return modifies_others_; }

bool Variable::excluded() const { return excluded_; }

VariablePtr findVariable(const string& id, const VariablesMapPtr& variables) {
  auto it = variables->find(id);
  if (it != variables->end()) {
    return it->second;
  }
  throw out_of_range(id + " variable does not exist");
}
} // namespace IODD
