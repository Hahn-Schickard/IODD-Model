#ifndef __IODD_STANDARD_DEFINES_NAMED_ATTRIBUTE_HPP
#define __IODD_STANDARD_DEFINES_NAMED_ATTRIBUTE_HPP

#include "TextID.hpp"

#include <memory>
#include <optional>
#include <stdexcept>

namespace IODD {
struct AttributeNotNamed : public std::runtime_error {
  AttributeNotNamed()
      : runtime_error("Requested attribute does not have a name") {}
};

struct NamedAttribute {
  NamedAttribute() = default;

  explicit NamedAttribute(const TextIDPtr& name) : name_(name) {}

  virtual ~NamedAttribute() = default;

  TextIDPtr tryName() {
    if (!name_) {
      throw AttributeNotNamed();
    }
    return name_;
  }

  TextIDPtr name() { return name_; }

private:
  TextIDPtr name_ = nullptr;
};

using NamedAttributePtr = std::shared_ptr<NamedAttribute>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_NAMED_ATTRIBUTE_HPP