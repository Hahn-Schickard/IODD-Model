#ifndef __IODD_STANDARD_DEFINES_RECORD_REF_HPP
#define __IODD_STANDARD_DEFINES_RECORD_REF_HPP

#include "VariableRef.hpp"

namespace IODD {

struct RecordRef : public VariableRef {
  RecordRef(const VariablePtr& variable,
      uint8_t subindex,
      ButtonValue value,
      const std::optional<TextID>& description = std::nullopt,
      const std::optional<TextID>& action_started_msg = std::nullopt);

  RecordRef(const VariablePtr& variable,
      uint8_t subindex,
      const std::optional<float>& gradient = std::nullopt,
      const std::optional<float>& offset = std::nullopt,
      const UnitPtr& unit = nullptr,
      DisplayFormat format = DisplayFormat::None,
      const std::optional<AccessRights>& access = std::nullopt);

  uint8_t subindex() const;

  NamedAttributePtr valueName(const SimpleDatatypeValue& value) const;

private:
  uint8_t subindex_;
};

using RecordRefPtr = std::shared_ptr<RecordRef>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_RECORD_REF_HPP