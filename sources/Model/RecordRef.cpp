#include "RecordRef.hpp"
#include "Variant_Visitor.hpp"

using namespace std;

namespace IODD {

RecordRef::RecordRef(const VariablePtr& variable,
    uint8_t subindex,
    ButtonValue value,
    const optional<TextID>& description,
    const optional<TextID>& action_started_msg)
    : VariableRef(variable, value, description, action_started_msg),
      subindex_(subindex) {}

RecordRef::RecordRef(const VariablePtr& variable,
    uint8_t subindex,
    const optional<float>& gradient,
    const optional<float>& offset,
    const UnitPtr& unit,
    DisplayFormat format,
    const optional<AccessRights>& access)
    : VariableRef(variable, gradient, offset, unit, format, access),
      subindex_(subindex) {}

uint8_t RecordRef::subindex() const { return subindex_; }

VariableRef::Value RecordRef::calculate(const VariableRef::Value& value) const {
  try {
    auto data_value = variable_->value();
    if (!holds_alternative<RecordT_Ptr>(data_value)) {
      throw std::logic_error("Variable value is not a Record Type");
    }
    auto record = get<RecordT_Ptr>(data_value);
    if (record->subindexAccess()) {
      auto item = record->item(subindex_);
    } else {
    }
  } catch (const logic_error& error) {
  }
}

NamedAttributePtr RecordRef::valueName(const SimpleDatatypeValue& value) const {
  return variable_->valueName(value, subindex_);
}
} // namespace IODD
