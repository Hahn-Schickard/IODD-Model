#include "Variable.hpp"

using namespace std;

namespace IODD {

template <typename T>
T getSimpleDatatypeValue(const SimpleDatatypeValue& variant) {
  if (auto* value = std::get_if<T>(&variant)) {
    return *value;
  } else {
    throw std::invalid_argument(
        "Simple value variant does not hold the requested type");
  }
}

template <typename T>
NamedAttributePtr getValueName(T value_type, const SimpleDatatypeValue& value) {
  throw std::runtime_error(
      "Given value type does not support named value attribute");
}

template <>
NamedAttributePtr getValueName<BooleanT>(
    BooleanT value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<bool>(value));
}

template <>
NamedAttributePtr getValueName<UIntegerT>(
    UIntegerT value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<uint64_t>(value));
}

template <>
NamedAttributePtr getValueName<IntegerT>(
    IntegerT value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<int64_t>(value));
}

template <>
NamedAttributePtr getValueName<FloatT>(
    FloatT value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<float>(value));
}

template <typename T>
NamedAttributePtr getValueName(RecordT<T> record,
    std::optional<uint8_t> subindex,
    const SimpleDatatypeValue& value) {
  if (record.subindexAccess()) {
    if (subindex.has_value()) {
      auto item = record.item(subindex.value());
      return getValueName<T>(item.value(), value);
    } else {
      throw std::invalid_argument("No subindex provided for record access");
    }
  } else {
    throw std::logic_error("Given record does not support subindex access");
  }
}

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
  // use raw pointer to avoid shared_ptr memory leak in lambda capture
  NamedAttribute* result = nullptr;
  match(
      value_,
      // Simple Types
      [&result, &value](const BooleanT& value_type) {
        auto ptr = move(getValueName<BooleanT>(value_type, value));
        result = ptr.get();
      },
      [&result, &value](const UIntegerT& value_type) {
        auto ptr = move(getValueName<UIntegerT>(value_type, value));
        result = ptr.get();
      },
      [&result, &value](const IntegerT& value_type) {
        auto ptr = move(getValueName<IntegerT>(value_type, value));
        result = ptr.get();
      },
      [&result, &value](const FloatT& value_type) {
        auto ptr = move(getValueName<FloatT>(value_type, value));
        result = ptr.get();
      },
      // Record Types
      [&result, &value, subindex](const RecordT<BooleanT>& value_type) {
        auto ptr = move(getValueName<BooleanT>(value_type, subindex, value));
        result = ptr.get();
      },
      [&result, &value, subindex](const RecordT<UIntegerT>& value_type) {
        auto ptr = move(getValueName<UIntegerT>(value_type, subindex, value));
        result = ptr.get();
      },
      [&result, &value, subindex](const RecordT<IntegerT>& value_type) {
        auto ptr = move(getValueName<IntegerT>(value_type, subindex, value));
        result = ptr.get();
      },
      [&result, &value, subindex](const RecordT<FloatT>& value_type) {
        auto ptr = move(getValueName<FloatT>(value_type, subindex, value));
        result = ptr.get();
      },
      // rest of types
      [&](auto) {
        throw logic_error(toString(type()) +
            " does not support named value attribute lookup");
      });
  if (result != nullptr) {
    // TODO: Does this cause seg faults?
    return shared_ptr<NamedAttribute>(result);
  } else {
    throw runtime_error("Given simple value does not have a matching name");
  }
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

} // namespace IODD
