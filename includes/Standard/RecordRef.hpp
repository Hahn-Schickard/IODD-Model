#ifndef __IODD_STANDARD_DEFINES_RECORD_REF_HPP
#define __IODD_STANDARD_DEFINES_RECORD_REF_HPP

#include "VariableRef.hpp"

namespace IODD {

struct RecordRef : public VariableRef {
  RecordRef(const VariablePtr& variable,
      ButtonValue value,
      std::optional<TextID> description,
      std::optional<TextID> action_started_msg,
      uint8_t subindex)
      : VariableRef(variable, value, description, action_started_msg),
        subindex_(subindex) {}

  RecordRef(const VariablePtr& variable,
      std::optional<float> gradient,
      std::optional<float> offset,
      UnitPtr unit,
      DisplayFormat format,
      std::optional<AccessRights> access,
      uint8_t subindex)
      : VariableRef(variable, gradient, offset, unit, format, access),
        subindex_(subindex) {}

  uint8_t subindex() const { return subindex_; }

  const NamedAttributePtr valueName(const SimpleDatatypeValue& value) const {
    return variable_->valueName(value, subindex_);
  }

private:
  uint8_t subindex_;
};

using RecordRefPtr = std::shared_ptr<RecordRef>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_RECORD_REF_HPP