#ifndef __IODD_STANDARD_DEFINES_CONDITION_HPP
#define __IODD_STANDARD_DEFINES_CONDITION_HPP

#include "Variable.hpp"

#include <cstdint>
#include <optional>

namespace IODD {
struct Condition {
  Condition(const VariablePtr& variable, uint8_t value);

  Condition(const VariablePtr& variable, uint8_t subindex, uint8_t value);

  Condition(const VariablePtr& variable,
      const std::optional<uint8_t>& subindex,
      uint8_t value);

  ~Condition() = default;

  VariablePtr variable() const;

  std::optional<uint8_t> subindex() const;

  uint8_t value() const;

private:
  VariablePtr variable_;
  std::optional<uint8_t> subindex_;
  uint8_t value_;
};

} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_CONDITION_HPP