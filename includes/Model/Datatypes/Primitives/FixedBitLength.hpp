#ifndef __IODD_STANDARD_DEFINES_FIXED_BIT_LENGTH_HPP
#define __IODD_STANDARD_DEFINES_FIXED_BIT_LENGTH_HPP

#include <cstdint>
#include <stdexcept>

namespace IODD {

template <size_t MIN, size_t MAX> struct FixedBitLength {
  FixedBitLength() = default;

  FixedBitLength(uint8_t bits) : bit_length_(bits) {
    if (bit_length_ < MIN) {
      throw std::invalid_argument(
          "Bit length can not be smaller than " + std::to_string(MIN));
    } else if (bit_length_ > MAX) {
      throw std::invalid_argument(
          "Bit length can not be larger than " + std::to_string(MAX));
    }
  }

  size_t bitLength() const { return bit_length_; }

private:
  size_t bit_length_;
};

} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_FIXED_BIT_LENGTH_HPP