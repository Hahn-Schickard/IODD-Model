#ifndef __IODD_STANDARD_DEFINES_OCTET_STRING_T_HPP
#define __IODD_STANDARD_DEFINES_OCTET_STRING_T_HPP

#include <cstdint>

namespace IODD {
struct OctetStringT {
  OctetStringT() = default;

  OctetStringT(size_t length) : length_(length) {}

  size_t hash() const noexcept { return length_; }

  size_t length() const { return length_; }

protected:
  size_t length_;
};

inline bool operator==(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() > rhs.hash();
}
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_OCTET_STRING_T_HPP