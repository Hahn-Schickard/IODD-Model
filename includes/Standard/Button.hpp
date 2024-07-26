#ifndef __IODD_STANDARD_DEFINES_BUTTON_HPP
#define __IODD_STANDARD_DEFINES_BUTTON_HPP

#include "Variable.hpp"

#include "Variant_Visitor.hpp"

#include <optional>
#include <stdexcept>

namespace IODD {

using ButtonValue = std::variant<bool, uint64_t, int64_t>;

Datatype toDatatype(ButtonValue value) {
  Datatype result;
  match(
      value,
      [&result](bool) { result = Datatype::Boolean; },
      [&result](uint64_t) { result = Datatype::UInteger; },
      [&result](int64_t) { result = Datatype::Integer; });
  return result;
}

struct Button {
  Button(const VariablePtr& variable,
      ButtonValue value,
      std::optional<TextID> description = std::nullopt,
      std::optional<TextID> action_started_msg = std::nullopt)
      : variable_(variable), value_(value), desc_(description),
        action_msg_(action_started_msg) {
    if (variable_->type() != Datatype::Boolean ||
        variable_->type() != Datatype::UInteger ||
        variable_->type() != Datatype::Integer) {
      throw std::invalid_argument(
          "Buttons must use BooleanT, UIntegerT or IntegerT variables");
    }
    if (variable_->type() != toDatatype(value_)) {
      throw std::invalid_argument(
          "Button variable type must match button value type");
    }
  }

  const VariablePtr variable() const { return variable_; }

  ButtonValue value() const { return value_; }

  std::optional<TextID> description() const { return desc_; }

  std::optional<TextID> actionMessage() const { return action_msg_; }

private:
  VariablePtr variable_;
  ButtonValue value_;
  std::optional<TextID> desc_ = std::nullopt;
  std::optional<TextID> action_msg_ = std::nullopt;
};
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_BUTTON_HPP