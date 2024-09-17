#ifndef __IODD_STANDARD_DEFINES_RECORD_REF_HPP
#define __IODD_STANDARD_DEFINES_RECORD_REF_HPP

#include "VariableRef.hpp"

namespace IODD {

struct RecordRef : public VariableRef {
  RecordRef(const VariablePtr& variable,
      ButtonValue value,
      const std::optional<TextID>& description,
      const std::optional<TextID>& action_started_msg,
      uint8_t subindex);

  RecordRef(const VariablePtr& variable,
      const std::optional<float>& gradient,
      const std::optional<float>& offset,
      const UnitPtr& unit,
      DisplayFormat format,
      const std::optional<AccessRights>& access,
      uint8_t subindex);

  uint8_t subindex() const;

  NamedAttributePtr valueName(const SimpleDatatypeValue& value) const;

private:
  uint8_t subindex_;
};

using RecordRefPtr = std::shared_ptr<RecordRef>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_RECORD_REF_HPP