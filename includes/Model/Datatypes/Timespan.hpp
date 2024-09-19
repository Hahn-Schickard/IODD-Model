#ifndef __IODD_STANDARD_DEFINES_TIMESPAN_T_HPP
#define __IODD_STANDARD_DEFINES_TIMESPAN_T_HPP

#include <cstdint>

namespace IODD {
struct TimeSpanT {
  size_t hash() const noexcept { return -2; }
};

inline bool operator==(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() > rhs.hash();
}
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_TIMESPAN_T_HPP