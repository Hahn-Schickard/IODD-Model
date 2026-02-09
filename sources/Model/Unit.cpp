#include "Unit.hpp"

using namespace std;

namespace IODD {

Unit::Unit(uint16_t code, const string& abbr) : code_(code), abbr_(abbr) {}

Unit::Unit(const TextIDPtr& name, uint16_t code, const string& abbr)
    : NamedAttribute(name), code_(code), abbr_(abbr) {}

uint16_t Unit::code() const { return code_; }

string Unit::abbr() const { return abbr_; }

} // namespace IODD
