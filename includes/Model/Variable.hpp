#ifndef __IODD_STANDARD_DEFINES_VARIABLE_HPP
#define __IODD_STANDARD_DEFINES_VARIABLE_HPP

#include "Datatypes/Datatypes.hpp"
#include "Datatypes/ProcessData.hpp"

#include <stdexcept>

namespace IODD {

struct VariableNotDescribed : public std::runtime_error {
  explicit VariableNotDescribed(size_t index)
      : runtime_error("Requested variable " + std::to_string(index) +
            " does not have a description") {}
};

struct Variable {
  Variable() = default;

  Variable(size_t index,
      const TextIDPtr& name,
      AccessRights access,
      DataValue&& value,
      const TextIDPtr& desc = nullptr,
      const std::optional<SimpleDatatypeValue>& default_value = std::nullopt,
      bool dynamic = false,
      bool modifies_others = false,
      bool excluded = false);

  Variable(const Variable& other,
      const std::optional<SimpleDatatypeValue>& default_value,
      const std::optional<bool>& excluded,
      const std::optional<DataValue>& value);

  Variable(size_t index,
      const TextIDPtr& name,
      AccessRights access,
      const ProcessDataTPtr& process_data = nullptr,
      const TextIDPtr& desc = nullptr,
      const std::optional<SimpleDatatypeValue>& default_value = std::nullopt,
      bool dynamic = false,
      bool modifies_others = false,
      bool excluded = false);

  Variable(const Variable& other, const ProcessDataTPtr& process_data);

  ~Variable() = default;

  size_t index() const;

  TextIDPtr name() const;

  AccessRights access() const;

  bool holdsProcessData() const;

  DataValue value() const;

  NamedAttributePtr valueName(const SimpleDatatypeValue& value,
      std::optional<uint8_t> subindex = std::nullopt) const;

  Datatype type() const;

  SimpleDatatypeValue defaultValue() const;

  TextIDPtr description() const;

  TextIDPtr tryDescription() const;

  bool dynamic() const;

  bool modifiesOthers() const;

  bool excluded() const;

private:
  size_t index_;
  TextIDPtr name_;
  AccessRights access_;
  std::optional<DataValue> value_;
  ProcessDataTPtr process_data_;
  TextIDPtr desc_;
  std::optional<SimpleDatatypeValue> default_;
  bool dynamic_;
  bool modifies_others_;
  bool excluded_;
};

using VariablePtr = std::shared_ptr<Variable>;
using VariablesMap = std::unordered_map<std::string, VariablePtr>;

VariablePtr findVariable(const std::string& id, const VariablesMap& variables);
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_VARIABLE_HPP