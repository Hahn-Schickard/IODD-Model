#ifndef __IODD_STANDARD_DEFINES_MENU_HPP
#define __IODD_STANDARD_DEFINES_MENU_HPP

#include "Unit.hpp"
#include "Variable.hpp"

#include "Variant_Visitor.hpp"

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace IODD {

enum class DisplayFormat : uint8_t {
  None = 20, /*!< Display format is not used */
  Bin = 10, /*!< Binary notation */
  Hex = 11, /*!< Hexadecimal notation */
  Dec0 = 0, /*!< Decimal notation without digits after the decimal point */
  Dec1 = 1, /*!< Decimal notation with 1 digit after the decimal point */
  Dec2 = 2, /*!< Decimal notation with 2 digits after the decimal point */
  Dec3 = 3, /*!< Decimal notation with 3 digits after the decimal point */
  Dec4 = 4, /*!< Decimal notation with 4 digits after the decimal point */
  Dec5 = 5, /*!< Decimal notation with 5 digits after the decimal point */
  Dec6 = 6, /*!< Decimal notation with 6 digits after the decimal point */
  Dec7 = 7, /*!< Decimal notation with 7 digits after the decimal point */
  Dec8 = 8, /*!< Decimal notation with 8 digits after the decimal point */
  Dec9 = 9 /*!< Decimal notation with 9 digits after the decimal point */
};

inline bool isDecimal(DisplayFormat format) {
  return static_cast<uint8_t>(format) < 10;
}

inline bool isBinary(DisplayFormat format) {
  return format == DisplayFormat::Bin;
}

inline bool isHexadecimal(DisplayFormat format) {
  return format == DisplayFormat::Hex;
}

struct Menu {
  using Value = std::variant<bool,
      uint64_t,
      int64_t,
      float,
      std::string,
      std::vector<uint8_t>>;

  Menu(const VariablePtr& variable)
      : variable_(variable), access_(variable->access()) {}

  Menu(const VariablePtr& variable, float gradient, float offset)
      : variable_(variable), gradient_(gradient), offset_(offset),
        access_(variable->access()) {}

  Menu(const VariablePtr& variable, UnitPtr unit, DisplayFormat format)
      : variable_(variable), unit_(unit), format_(format),
        access_(variable->access()) {}

  Menu(const VariablePtr& variable,
      float gradient,
      float offset,
      UnitPtr unit,
      DisplayFormat format)
      : variable_(variable), gradient_(gradient), offset_(offset), unit_(unit),
        format_(format), access_(variable->access()) {}

  Menu(const VariablePtr& variable,
      float gradient,
      float offset,
      UnitPtr unit,
      DisplayFormat format,
      AccessRights access)
      : variable_(variable), gradient_(gradient), offset_(offset), unit_(unit),
        format_(format), access_(access) {}

  Menu(const VariablePtr& variable,
      uint16_t subindex,
      float gradient,
      float offset,
      UnitPtr unit,
      DisplayFormat format)
      : variable_(variable), subindex_(subindex), gradient_(gradient),
        offset_(offset), unit_(unit), format_(format),
        access_(variable->access()) {}

  Menu(const VariablePtr& variable,
      uint16_t subindex,
      float gradient,
      float offset,
      UnitPtr unit,
      DisplayFormat format,
      AccessRights access)
      : variable_(variable), subindex_(subindex), gradient_(gradient),
        offset_(offset), unit_(unit), format_(format), access_(access) {}

  Value value(const Value& value) const {
    checkValueType(value);
    if (isDecimal(format_)) {
      if (isNumericData(variable_->type())) {
        return decodeNumeric(value);
      }
      // TODO: add Record[subindex] decoding
    }
    // value does not need to be decoded, return it as is
    return value;
  }

  const NamedAttributePtr valueName(const SimpleValue& value) {
    return variable_->valueName(value);
  }

  DisplayFormat displayFormat() const { return format_; }

  AccessRights accessRestriction() const { return access_; }

  std::optional<uint16_t> subindex() const { return subindex_; }

  const UnitPtr unit() const { return unit_; }

  const VariablePtr variable() const { return variable_; }

private:
  float decodeNumeric(const Value& value) const {
    float f_value;
    match(
        value,
        [&f_value](bool) {
          throw std::logic_error(
              "Decoded result can not be represented as boolean");
        },
        [&f_value](const std::string&) {
          throw std::logic_error(
              "Decoded result can not be represented as string");
        },
        [&f_value](uint64_t number) { f_value = number; },
        [&f_value](int64_t number) { f_value = number; },
        [&f_value](float number) { f_value = number; });
    return (f_value * gradient_) + offset_;
  }

  void checkValueType(const Value& value) const {
    switch (variable_->type()) {
    case Datatype::Boolean: {
      if (!std::holds_alternative<bool>(value)) {
        throw std::invalid_argument(
            "Non bool value type passed to Boolean variable decoder");
      }
      break;
    }
    case Datatype::UInteger: {
      if (!std::holds_alternative<uint64_t>(value)) {
        throw std::invalid_argument(
            "Non unsigned int value type passed to UInteger variable decoder");
      }
    }
    case Datatype::Integer: {
      if (!std::holds_alternative<int64_t>(value)) {
        throw std::invalid_argument(
            "Non int value type passed to Integer variable decoder");
      }
      break;
    }
    case Datatype::Float32: {
      if (!std::holds_alternative<float>(value)) {
        throw std::invalid_argument(
            "Non float value type passed to Float32 variable decoder");
      }
      break;
    }
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
      if (!std::holds_alternative<std::string>(value)) {
        throw std::invalid_argument("Non string value type passed to " +
            toString(variable_->type()) + " variable decoder");
      }
      break;
    }
    case Datatype::Array: {
      [[fallthrough]];
    }
    case Datatype::Record: {
      if (!std::holds_alternative<std::vector<uint8_t>>(value)) {
        throw std::invalid_argument("Non byte vector value type passed to " +
            toString(variable_->type()) + " variable decoder");
      }
      break;
    }
    case Datatype::ProcessDataIn: {
      [[fallthrough]];
    }
    case Datatype::ProcessDataOut: {
      // can be any type?
      break;
    }
    }
  }

  VariablePtr variable_;
  std::optional<uint16_t> subindex_ = std::nullopt; // for records only
  float gradient_ = 1.0;
  float offset_ = 0.0;
  UnitPtr unit_ = nullptr;
  DisplayFormat format_ = DisplayFormat::None;
  AccessRights access_; // defaults to be same as variable
};

using MenuPtr = std::shared_ptr<Menu>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_MENU_HPP