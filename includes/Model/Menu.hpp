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
      const std::optional<uint8_t>& subindex,
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
  using Refs = std::vector<Ref>;

  Menu(const std::string& id_,
      const Refs& references_,
      const std::optional<TextID>& name_ = std::nullopt,
      const std::optional<Condition>& condition_ = std::nullopt);

  size_t size() const;

  std::string id() const;

  Refs references() const;

  std::optional<TextID> name() const;

  std::optional<Condition> condition() const;

private:
  std::string id_;
  Refs references_;
  std::optional<TextID> name_;
  std::optional<Condition> condition_;
};

using MenuPtr = std::shared_ptr<Menu>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_MENU_HPP
