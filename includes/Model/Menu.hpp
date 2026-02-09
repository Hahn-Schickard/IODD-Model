#ifndef __IODD_STANDARD_DEFINES_MENU_HPP
#define __IODD_STANDARD_DEFINES_MENU_HPP

#include "Condition.hpp"
#include "RecordRef.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>

namespace IODD {

struct MenuHasNoName : public std::runtime_error {
  explicit MenuHasNoName(const std::string& id)
      : runtime_error("Requested menu " + id + " is not named") {}
};

struct Menu {
  using Ref = std::variant<VariableRefPtr, RecordRefPtr, std::shared_ptr<Menu>>;
  using Refs = std::vector<Ref>;

  Menu(const std::string& id,
      Refs&& references,
      const TextIDPtr& name = nullptr,
      const std::optional<Condition>& condition = std::nullopt);

  ~Menu() = default;

  size_t size() const;

  std::string id() const;

  Refs references() const;

  TextIDPtr name() const;

  TextIDPtr tryName() const;

  std::optional<Condition> condition() const;

private:
  std::string id_;
  Refs references_;
  TextIDPtr name_;
  std::optional<Condition> condition_;
};

using MenuPtr = std::shared_ptr<Menu>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_MENU_HPP
