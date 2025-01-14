#ifndef __IODD_STANDARD_DEFINES_TIME_T_HPP
#define __IODD_STANDARD_DEFINES_TIME_T_HPP

#include <cstdint>

namespace IODD {

struct TimeT {
  size_t hash() const noexcept { return -1; }

  size_t length() const { return 8; }
};

using TimeT_Ptr = std::shared_ptr<TimeT>;

inline bool operator==(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() > rhs.hash();
}

} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_TIME_T_HPP