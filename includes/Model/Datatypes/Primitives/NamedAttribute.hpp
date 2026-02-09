#ifndef __IODD_STANDARD_DEFINES_NAMED_ATTRIBUTE_HPP
#define __IODD_STANDARD_DEFINES_NAMED_ATTRIBUTE_HPP

#include "TextID.hpp"

#include <memory>
#include <optional>

namespace IODD {

struct NamedAttribute {
  NamedAttribute() = default;

  explicit NamedAttribute(std::optional<TextID>&& name)
      : name_(std::move(name)) {}

  ~NamedAttribute() = default;

  std::optional<TextID> name() { return name_; }

private:
  std::optional<TextID> name_ = std::nullopt;
};

using NamedAttributePtr = std::shared_ptr<NamedAttribute>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_NAMED_ATTRIBUTE_HPP