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
  if (value_) {
    if (value) {
      expand(value_.value(), *value);
    }
  } else {
    throw logic_error("Can not expand variable " + to_string(index_) + " " +
        name_.locale() + " DataValue type. It uses ProcessDataT");
  }
}

Variable::Variable(size_t index,
    TextID&& name,
    AccessRights access,
    const ProcessDataTPtr& process_data,
    optional<TextID>&& desc,
    optional<SimpleDatatypeValue> default_value,
    bool dynamic,
    bool modifies_others,
    bool excluded)
    : index_(index), name_(move(name)), access_(access),
      process_data_(process_data), desc_(move(desc)),
      default_(move(default_value)), dynamic_(dynamic),
      modifies_others_(modifies_others), excluded_(excluded) {}

Variable::Variable(const Variable& other, const ProcessDataTPtr& process_data)
    : index_(other.index_), name_(other.name_), access_(other.access_),
      process_data_(process_data), desc_(other.desc_), dynamic_(other.dynamic_),
      modifies_others_(other.modifies_others_), excluded_(other.excluded_) {
  if (!process_data_) {
    throw invalid_argument("ProcessDataTPtr can not be empty");
  }
}

size_t Variable::index() const { return index_; }

TextID Variable::name() const { return name_; }

AccessRights Variable::access() const { return access_; }

bool Variable::holdsProcessData() const {
  // if no DataValue was set, variable is a placeholder for ProcessData
  return !value_.has_value();
}

DataValue Variable::value() const {
  if (value_) {
    return *value_;
  } else if (process_data_) {
    return process_data_->value();
  } else {
    throw runtime_error("Variable " + to_string(index_) + " " + name_.locale() +
        " does not use DataValue type");
  }
}

template <class T> T Variable::variantValue() const {
  if (value_) {
    return get<T>(value_);
  } else {
    throw runtime_error("Variable " + to_string(index_) + " " + name_.locale() +
        " does not use DataValue type, thus not variant value could be "
        "returned");
  }
}

NamedAttributePtr Variable::valueName(
    const SimpleDatatypeValue& value, optional<uint8_t> subindex) const {
  if (value_) {
    return getValueName(*value_, value, subindex);
  } else if (process_data_) {
    return process_data_->valueName(value, subindex);
  } else {
    throw runtime_error("Can not get value name from variable " +
        to_string(index_) + " " + name_.locale() +
        ". Variable does not have a value type");
  }
}

Datatype Variable::type() const {
  if (value_) {
    return toDatatype(*value_);
  } else if (process_data_) {
    return process_data_->type();
  } else {
    throw runtime_error("Can not get variable datatype " + to_string(index_) +
        " " + name_.locale() + ". Variable does not have a value type");
  }
}

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

VariablePtr findVariable(const string& id, const VariablesMap& variables) {
  auto it = variables.find(id);
  if (it != variables.end()) {
    return it->second;
  }
  throw out_of_range(id + " variable does not exist");
}
} // namespace IODD
