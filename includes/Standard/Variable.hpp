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
NamedAttributePtr getValueName<BooleanT>(
    BooleanT value_type, const SimpleDatatypeValue& value);

template <>
NamedAttributePtr getValueName<UIntegerT>(
    UIntegerT value_type, const SimpleDatatypeValue& value);

template <>
NamedAttributePtr getValueName<IntegerT>(
    IntegerT value_type, const SimpleDatatypeValue& value);

template <>
NamedAttributePtr getValueName<FloatT>(
    FloatT value_type, const SimpleDatatypeValue& value);

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
      bool excluded = false);

  Variable(const Variable& other,
      std::optional<SimpleDatatypeValue> default_value,
      std::optional<bool> excluded,
      std::optional<DataValue> value);

  size_t index() const;

  TextID name() const;

  AccessRights access() const;

  DataValue value() const;

  NamedAttributePtr valueName(const SimpleDatatypeValue& value,
      std::optional<uint8_t> subindex = std::nullopt);

  Datatype type() const;

  SimpleDatatypeValue defaultValue() const;

  std::optional<TextID> description() const;

  bool dynamic() const;

  bool modifiesOthers() const;

  bool excluded() const;

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