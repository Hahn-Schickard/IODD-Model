#ifndef __IODD_STANDARD_DEFINES_VARIABLE_HPP
#define __IODD_STANDARD_DEFINES_VARIABLE_HPP

#include "Datatypes/Datatypes.hpp"

namespace IODD {

using SimpleDatatypeValue = // TimeT and TimeSpanT are stored as strings
    std::variant<bool, uint64_t, int64_t, float, std::string>;

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
inline NamedAttributePtr getValueName<BooleanT>(
    BooleanT value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<bool>(value));
}

template <>
inline NamedAttributePtr getValueName<UIntegerT>(
    UIntegerT value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<uint64_t>(value));
}

template <>
inline NamedAttributePtr getValueName<IntegerT>(
    IntegerT value_type, const SimpleDatatypeValue& value) {
  return value_type.getName(getSimpleDatatypeValue<int64_t>(value));
}

template <>
inline NamedAttributePtr getValueName<FloatT>(
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

struct Variable {
  Variable() = default;

  Variable(size_t index,
      TextID&& name,
      AccessRights access,
      DataValue&& value,
      std::optional<TextID>&& desc = std::nullopt,
      std::optional<SimpleDatatypeValue> default_value = std::nullopt,
      bool dynamic = false,
      bool modifies_others = false,
      bool excluded = false)
      : index_(index), name_(std::move(name)), access_(access),
        value_(std::move(value)), desc_(std::move(desc)),
        default_(std::move(default_value)), dynamic_(dynamic),
        modifies_others_(modifies_others), excluded_(excluded) {}

  Variable(const Variable& other,
      std::optional<SimpleDatatypeValue> default_value,
      std::optional<bool> excluded,
      std::optional<DataValue> value)
      : index_(other.index_), name_(other.name_), access_(other.access_),
        value_(other.value_), desc_(other.desc_),
        default_((default_value ? default_value.value() : other.default_)),
        dynamic_(other.dynamic_), modifies_others_(other.modifies_others_),
        excluded_((excluded ? excluded.value() : other.excluded_)) {
    if (value.has_value()) {
      expand(value_, *value);
    }
  }

  size_t index() const { return index_; }

  TextID name() const { return name_; }

  AccessRights access() const { return access_; }

  DataValue value() const { return value_; }

  NamedAttributePtr valueName(const SimpleDatatypeValue& value,
      std::optional<uint8_t> subindex = std::nullopt) const {
    // use raw pointer to avoid shared_ptr memory leak in lambda capture
    NamedAttribute* result = nullptr;
    match(
        value_,
        // Simple Types
        [&result, &value](const BooleanT& value_type) {
          auto ptr = std::move(getValueName<BooleanT>(value_type, value));
          result = ptr.get();
        },
        [&result, &value](const UIntegerT& value_type) {
          auto ptr = std::move(getValueName<UIntegerT>(value_type, value));
          result = ptr.get();
        },
        [&result, &value](const IntegerT& value_type) {
          auto ptr = std::move(getValueName<IntegerT>(value_type, value));
          result = ptr.get();
        },
        [&result, &value](const FloatT& value_type) {
          auto ptr = std::move(getValueName<FloatT>(value_type, value));
          result = ptr.get();
        },
        // Record Types
        [&result, &value, subindex](const RecordT<BooleanT>& value_type) {
          auto ptr =
              std::move(getValueName<BooleanT>(value_type, subindex, value));
          result = ptr.get();
        },
        [&result, &value, subindex](const RecordT<UIntegerT>& value_type) {
          auto ptr =
              std::move(getValueName<UIntegerT>(value_type, subindex, value));
          result = ptr.get();
        },
        [&result, &value, subindex](const RecordT<IntegerT>& value_type) {
          auto ptr =
              std::move(getValueName<IntegerT>(value_type, subindex, value));
          result = ptr.get();
        },
        [&result, &value, subindex](const RecordT<FloatT>& value_type) {
          auto ptr =
              std::move(getValueName<FloatT>(value_type, subindex, value));
          result = ptr.get();
        },
        // rest of types
        [&](auto) {
          throw std::logic_error(toString(type()) +
              " does not support named value attribute lookup");
        });
    if (result != nullptr) {
      // TODO: Does this cause seg faults?
      return std::shared_ptr<NamedAttribute>(result);
    } else {
      throw std::runtime_error(
          "Given simple value does not have a matching name");
    }
  }

  Datatype type() const { return toDatatype(value_); }

  SimpleDatatypeValue defaultValue() const {
    if (default_.has_value()) {
      return default_.value();
    } else {
      throw std::runtime_error(name_.id() + " Variable has no default value");
    }
  }

  std::optional<TextID> description() const { return desc_; }

  bool dynamic() const { return dynamic_; }

  bool modifiesOthers() const { return modifies_others_; }

  bool excluded() const { return excluded_; }

private:
  size_t index_;
  TextID name_;
  AccessRights access_;
  DataValue value_;
  std::optional<TextID> desc_;
  std::optional<SimpleDatatypeValue> default_;
  bool dynamic_;
  bool modifies_others_;
  bool excluded_;
};

using VariablePtr = std::shared_ptr<Variable>;
using VariablesMap = std::unordered_map<std::string, VariablePtr>;
using VariablesMapPtr = std::shared_ptr<VariablesMap>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_VARIABLE_HPP