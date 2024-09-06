#ifndef __IODD_STANDARD_DEFINES_MENU_HPP
#define __IODD_STANDARD_DEFINES_MENU_HPP

#include "RecordRef.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace IODD {

struct Condition {
  Condition(const VariablePtr& variable, uint8_t value)
      : variable_(variable), value_(value) {}

  Condition(const VariablePtr& variable, uint8_t subindex, uint8_t value)
      : variable_(variable), subindex_(subindex), value_(value) {}

  Condition(const VariablePtr& variable,
      std::optional<uint8_t> subindex,
      uint8_t value)
      : variable_(variable), subindex_(subindex), value_(value) {}

  VariablePtr variable() { return variable_; }

  std::optional<uint8_t> subindex() { return subindex_; }

  uint8_t value() { return value_; }

private:
  VariablePtr variable_;
  std::optional<uint8_t> subindex_;
  uint8_t value_;
};

struct Menu {
  using Ref = std::variant<VariableRefPtr, RecordRefPtr, std::shared_ptr<Menu>>;

  Menu(const std::string& _id,
      const std::vector<Ref>& _references,
      const std::optional<TextID>& _name = std::nullopt,
      const std::optional<Condition> _condition = std::nullopt)
      : id(_id), references(_references), name(_name), condition(_condition) {}

  const std::string id;
  const std::vector<Ref> references;
  const std::optional<TextID> name;
  const std::optional<Condition> condition;
};

using MenuPtr = std::shared_ptr<Menu>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_MENU_HPP