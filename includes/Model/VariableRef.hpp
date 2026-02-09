#ifndef __IODD_STANDARD_DEFINES_VARIABLE_REF_HPP
#define __IODD_STANDARD_DEFINES_VARIABLE_REF_HPP

#include "Unit.hpp"
#include "Variable.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace IODD {

enum class DisplayFormat : uint8_t {
  None = 30, /*!< Display format is not used */
  Bin = 20, /*!< Binary notation */
  Hex = 21, /*!< Hexadecimal notation */
  Dec0 = 0, /*!< Decimal notation without digits after the decimal point */
  Dec1 = 1, /*!< Decimal notation with 1 digit after the decimal point */
  Dec2 = 2, /*!< Decimal notation with 2 digits after the decimal point */
  Dec3 = 3, /*!< Decimal notation with 3 digits after the decimal point */
  Dec4 = 4, /*!< Decimal notation with 4 digits after the decimal point */
  Dec5 = 5, /*!< Decimal notation with 5 digits after the decimal point */
  Dec6 = 6, /*!< Decimal notation with 6 digits after the decimal point */
  Dec7 = 7, /*!< Decimal notation with 7 digits after the decimal point */
  Dec8 = 8, /*!< Decimal notation with 8 digits after the decimal point */
  Dec9 = 9, /*!< Decimal notation with 9 digits after the decimal point */
  Dec = 10 /*!< Display decimal point digits as needed*/
};

bool isDecimal(DisplayFormat format);

bool isBinary(DisplayFormat format);

bool isHexadecimal(DisplayFormat format);

struct VariableRefHasNoActionMsg : public std::runtime_error {
  VariableRefHasNoActionMsg()
      : runtime_error(
            "Requested variable reference does not have an action message") {}
};

struct VariableRef {
  using Value = std::variant<bool,
      uint64_t,
      int64_t,
      float,
      std::string,
      std::vector<uint8_t>>;
  using ButtonValue = std::variant<bool, uint64_t, int64_t>;

  VariableRef(const VariablePtr& variable,
      ButtonValue value,
      const TextIDPtr& description = nullptr,
      const TextIDPtr& action_started_msg = nullptr);

  explicit VariableRef(const VariablePtr& variable,
      const std::optional<float>& gradient = std::nullopt,
      const std::optional<float>& offset = std::nullopt,
      const UnitPtr& unit = nullptr,
      DisplayFormat format = DisplayFormat::None,
      const std::optional<AccessRights>& access = std::nullopt);

  virtual ~VariableRef() = default;

  virtual SimpleDatatypeValue decode(const std::vector<uint8_t>& bytes) const;

  virtual NamedAttributePtr valueName(const SimpleDatatypeValue& value) const;

  DisplayFormat displayFormat() const;

  AccessRights accessRestriction() const;

  UnitPtr unit() const;

  VariablePtr variable() const;

  bool isButton() const;

  ButtonValue buttonValue() const;

  TextIDPtr description() const;

  TextIDPtr tryDescription() const;

  TextIDPtr actionMessage() const;

  TextIDPtr tryActionMessage() const;

  float gradient() const;

  float offset() const;

protected:
  SimpleDatatypeValue decode(const SimpleDatatypeValue& value) const;

  VariablePtr variable_;

private:
  std::optional<ButtonValue> value_ = std::nullopt;
  TextIDPtr desc_ = nullptr;
  TextIDPtr action_msg_ = nullptr;

  float gradient_ = 1.0;
  float offset_ = 0.0;
  UnitPtr unit_ = nullptr;
  DisplayFormat format_ = DisplayFormat::None;
  AccessRights access_; // defaults to be same as variable
};

using VariableRefPtr = std::shared_ptr<VariableRef>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_VARIABLE_REF_HPP