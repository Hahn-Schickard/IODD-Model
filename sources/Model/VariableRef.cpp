#include "VariableRef.hpp"

using namespace std;

namespace IODD {

bool isDecimal(DisplayFormat format) {
  return static_cast<uint8_t>(format) <=
      static_cast<uint8_t>(DisplayFormat::Dec);
}

bool isBinary(DisplayFormat format) { return format == DisplayFormat::Bin; }

bool isHexadecimal(DisplayFormat format) {
  return format == DisplayFormat::Hex;
}

Datatype buttonValueToDatatype(const VariableRef::ButtonValue& value) {
  Datatype result;
  match(
      value,
      [&result](bool) { result = Datatype::Boolean; },
      [&result](uint64_t) { result = Datatype::UInteger; },
      [&result](int64_t) { result = Datatype::Integer; });
  return result;
}

void checkValueType(
    const VariablePtr& variable, const VariableRef::Value& value) {
  switch (variable->type()) {
  case Datatype::Boolean: {
    if (!holds_alternative<bool>(value)) {
      throw invalid_argument(
          "Non bool value type passed to Boolean variable decoder");
    }
    break;
  }
  case Datatype::UInteger: {
    if (!holds_alternative<uint64_t>(value)) {
      throw invalid_argument(
          "Non unsigned int value type passed to UInteger variable decoder");
    }
    break;
  }
  case Datatype::Integer: {
    if (!holds_alternative<int64_t>(value)) {
      throw invalid_argument(
          "Non int value type passed to Integer variable decoder");
    }
    break;
  }
  case Datatype::Float32: {
    if (!holds_alternative<float>(value)) {
      throw invalid_argument(
          "Non float value type passed to Float32 variable decoder");
    }
    break;
  }
  // NOLINTNEXTLINE(bugprone-branch-clone)
  case Datatype::String: {
    [[fallthrough]];
  }
  case Datatype::OctetString: {
    [[fallthrough]];
  }
  case Datatype::Time: {
    [[fallthrough]];
  }
  case Datatype::TimeSpan: {
    if (!holds_alternative<string>(value)) {
      throw invalid_argument("Non string value type passed to " +
          toString(variable->type()) + " variable decoder");
    }
    break;
  }
  default: {
    if (!holds_alternative<vector<uint8_t>>(value)) {
      throw invalid_argument("Non byte vector value type passed to " +
          toString(variable->type()) + " variable decoder");
    }
    break;
  }
  }
}

float computeNumeric(
    const VariableRef::Value& value, float gradient, float offset) {
  float f_value;
  match(
      value,
      [](bool) {
        throw logic_error("Decoded result can not be represented as boolean");
      },
      [](const string&) {
        throw logic_error("Decoded result can not be represented as string");
      },
      [](const vector<uint8_t>&) {
        throw logic_error(
            "Decoded result can not be represented as a byte array");
      },
      [&f_value](uint64_t number) { f_value = static_cast<float>(number); },
      [&f_value](int64_t number) { f_value = static_cast<float>(number); },
      [&f_value](float number) { f_value = number; });
  return (f_value * gradient) + offset;
}

VariableRef::VariableRef(const VariablePtr& variable,
    ButtonValue value,
    const optional<TextID>& description,
    const optional<TextID>& action_started_msg)
    : variable_(variable), value_(value), desc_(description),
      action_msg_(action_started_msg) {
  if (variable_->type() != Datatype::Boolean &&
      variable_->type() != Datatype::UInteger &&
      variable_->type() != Datatype::Integer) {
    throw invalid_argument("Buttons must use BooleanT, UIntegerT or IntegerT "
                           "variables types. Given Variable type is: " +
        toString(variable_->type()));
  }
  if (variable_->type() != buttonValueToDatatype(value_.value())) {
    throw invalid_argument("Button variable type must match button value type");
  }
}

VariableRef::VariableRef(const VariablePtr& variable,
    const optional<float>& gradient,
    const optional<float>& offset,
    const UnitPtr& unit,
    DisplayFormat format,
    const optional<AccessRights>& access)
    : variable_(variable), unit_(unit), format_(format) {
  if (gradient.has_value()) {
    gradient_ = gradient.value();
  }
  if (offset.has_value()) {
    offset_ = offset.value();
  }
  if (access.has_value()) {
    access_ = access.value();
  }
}

VariableRef::Value VariableRef::calculate(
    const VariableRef::Value& value) const {
  // @TODO: use std::vector<uint8_t>& and Decoders.hpp
  checkValueType(variable_, value);
  if (isDecimal(format_)) {
    if (isNumericData(variable_->type())) {
      return computeNumeric(value, gradient_, offset_);
    }
  }
  // value does not need to be calculated, return it as is
  return value;
}

NamedAttributePtr VariableRef::valueName(
    const SimpleDatatypeValue& value) const {
  return variable_->valueName(value);
}

DisplayFormat VariableRef::displayFormat() const { return format_; }

AccessRights VariableRef::accessRestriction() const { return access_; }

UnitPtr VariableRef::unit() const { return unit_; }

VariablePtr VariableRef::variable() const { return variable_; }

bool VariableRef::isButton() const { return value_.has_value(); }

VariableRef::ButtonValue VariableRef::buttonValue() const {
  if (value_.has_value()) {
    return value_.value();
  } else {
    throw runtime_error(
        variable_->name().locale() + " VariableRef is not a button");
  }
}

optional<TextID> VariableRef::description() const { return desc_; }

optional<TextID> VariableRef::actionMessage() const { return action_msg_; }

float VariableRef::gradient() const { return gradient_; }

float VariableRef::offset() const { return offset_; }

} // namespace IODD
