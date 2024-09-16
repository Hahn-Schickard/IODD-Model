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
  Condition(const VariablePtr& variable, uint8_t value);

  Condition(const VariablePtr& variable, uint8_t subindex, uint8_t value);

  Condition(const VariablePtr& variable,
      std::optional<uint8_t> subindex,
      uint8_t value);

  VariablePtr variable() const;

  std::optional<uint8_t> subindex() const;

  uint8_t value() const;

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
      const std::optional<Condition> _condition = std::nullopt);

  size_t size() const;

  const std::string id;
  const std::vector<Ref> references;
  const std::optional<TextID> name;
  const std::optional<Condition> condition;
};

using MenuPtr = std::shared_ptr<Menu>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_MENU_HPP
