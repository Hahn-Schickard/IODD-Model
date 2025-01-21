#ifndef __IODD_STANDARD_DEFINES_TIMESPAN_T_HPP
#define __IODD_STANDARD_DEFINES_TIMESPAN_T_HPP

#include <cstdint>

namespace IODD {
struct TimeSpanT {
  size_t length() const { return 8; }
};

using TimeSpanT_Ptr = std::shared_ptr<TimeSpanT>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_TIMESPAN_T_HPP
