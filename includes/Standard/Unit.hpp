#ifndef __IODD_STANDARD_DEFINES_UNIT_HPP
#define __IODD_STANDARD_DEFINES_UNIT_HPP

#include "Datatypes/Primitives/NamedAttribute.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace IODD {

struct Unit : public NamedAttribute {
  Unit() = default;

  Unit(uint16_t code, const std::string& abbr) : code_(code), abbr_(abbr) {}

  Unit(uint16_t code, const std::string& abbr, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), code_(code), abbr_(abbr) {}

  size_t hash() const noexcept { return std::hash<uint16_t>{}(code_); }

  uint16_t code() const { return code_; }

  std::string abbr() const { return abbr_; }

private:
  uint16_t code_;
  std::string abbr_;
};

using UnitPtr = std::shared_ptr<Unit>;
using UnitsMap = std::unordered_map<uint16_t, UnitPtr>;
using UnitsMapPtr = std::shared_ptr<UnitsMap>;

inline bool operator==(const Unit& lhs, const Unit& rhs) {
  return lhs.code() == rhs.code();
}

inline bool operator!=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() != rhs.code();
}

inline bool operator<=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() <= rhs.code();
}

inline bool operator>=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() >= rhs.code();
}

inline bool operator<(const Unit& lhs, const Unit& rhs) {
  return lhs.code() < rhs.code();
}

inline bool operator>(const Unit& lhs, const Unit& rhs) {
  return lhs.code() > rhs.code();
}

} // namespace IODD

template <> struct std::hash<IODD::Unit> {
  std::size_t operator()(const IODD::Unit& object) const noexcept {
    return object.hash();
  }
};
#endif //__IODD_STANDARD_DEFINES_UNIT_HPP