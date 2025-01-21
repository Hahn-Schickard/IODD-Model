#ifndef __IODD_STANDARD_DEFINES_UNIT_HPP
#define __IODD_STANDARD_DEFINES_UNIT_HPP

#include "Datatypes/Primitives/NamedAttribute.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace IODD {

struct Unit : public NamedAttribute {
  Unit() = default;

  Unit(uint16_t code, const std::string& abbr);

  Unit(uint16_t code, const std::string& abbr, std::optional<TextID>&& name);

  uint16_t code() const;

  std::string abbr() const;

private:
  uint16_t code_;
  std::string abbr_;
};

using UnitPtr = std::shared_ptr<Unit>;
using UnitsMap = std::unordered_map<uint16_t, UnitPtr>;
using UnitsMapPtr = std::shared_ptr<UnitsMap>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_UNIT_HPP