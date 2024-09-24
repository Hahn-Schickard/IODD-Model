#ifndef __IODD_STANDARD_DEFINES_VARIABLE_HPP
#define __IODD_STANDARD_DEFINES_VARIABLE_HPP

#include "Datatypes/Datatypes.hpp"

namespace IODD {

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
      std::optional<uint8_t> subindex = std::nullopt) const;

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