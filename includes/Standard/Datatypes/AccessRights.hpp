#ifndef __IODD_STANDARD_DEFINES_ACCESS_RIGHTS_HPP
#define __IODD_STANDARD_DEFINES_ACCESS_RIGHTS_HPP

#include <string>

namespace IODD {

enum class AccessRights { ReadOnly, WriteOnly, ReadWrite };

inline std::string toString(AccessRights rights) {
  switch (rights) {
  case AccessRights::ReadOnly: {
    return "read only";
  }
  case AccessRights::WriteOnly: {
    return "write only";
  }
  case AccessRights::ReadWrite: {
    return "read & write";
  }
  }
}
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_ACCESS_RIGHTS_HPP