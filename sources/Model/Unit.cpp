#include "Unit.hpp"

using namespace std;

namespace IODD {

Unit::Unit(uint16_t code, const string& abbr) : code_(code), abbr_(abbr) {}

Unit::Unit(uint16_t code, const string& abbr, optional<TextID>&& name)
    : NamedAttribute(move(name)), code_(code), abbr_(abbr) {}

size_t Unit::hash() const noexcept { return std::hash<uint16_t>{}(code_); }

uint16_t Unit::code() const { return code_; }

string Unit::abbr() const { return abbr_; }

bool operator==(const Unit& lhs, const Unit& rhs) {
  return lhs.code() == rhs.code();
}

bool operator!=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() != rhs.code();
}

bool operator<=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() <= rhs.code();
}

bool operator>=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() >= rhs.code();
}

bool operator<(const Unit& lhs, const Unit& rhs) {
  return lhs.code() < rhs.code();
}

bool operator>(const Unit& lhs, const Unit& rhs) {
  return lhs.code() > rhs.code();
}

} // namespace IODD

size_t hash<IODD::Unit>::operator()(const IODD::Unit& object) const noexcept {
  return object.hash();
}
