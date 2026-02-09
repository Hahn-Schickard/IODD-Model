#ifndef __IODD_STANDARD_DEFINES_OCTET_STRING_T_HPP
#define __IODD_STANDARD_DEFINES_OCTET_STRING_T_HPP

#include <cstdint>
#include <stdexcept>

namespace IODD {
struct OctetStringT {
  OctetStringT() = default;

  explicit OctetStringT(size_t length) : length_(length) {
    constexpr uint8_t MAX_STRING_SIZE = 232;
    if (length > MAX_STRING_SIZE) {
      throw std::invalid_argument("Strings can not exceed 232 bytes in length");
    }
  }

  ~OctetStringT() = default;

  size_t length() const { return length_; }

protected:
  size_t length_;
};

using OctetStringT_Ptr = std::shared_ptr<OctetStringT>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_OCTET_STRING_T_HPP
