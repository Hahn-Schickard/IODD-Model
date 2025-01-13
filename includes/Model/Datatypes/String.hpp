#ifndef __IODD_STANDARD_DEFINES_STRING_T_HPP
#define __IODD_STANDARD_DEFINES_STRING_T_HPP

#include "OctetString.hpp"

namespace IODD {

struct StringT : public OctetStringT {
  StringT() = default;

  StringT(size_t length, bool utf = true) : OctetStringT(length), utf_(utf) {}

  size_t hash() const noexcept { return (length_ < 1) | utf_; }

  bool utf() const { return utf_; }

private:
  bool utf_; // US-ASCII if false
};

using StringT_Ptr = std::shared_ptr<StringT>;

} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_STRING_T_HPP