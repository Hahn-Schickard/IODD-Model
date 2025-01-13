#ifndef __IODD_STANDARD_DEFINES_COMPLEX_DATA_TYPE_HPP
#define __IODD_STANDARD_DEFINES_COMPLEX_DATA_TYPE_HPP

#include <type_traits>

namespace IODD {
struct ComplexDataTypeT {
  ComplexDataTypeT() = default;

  ComplexDataTypeT(bool subindex_access_support)
      : subindex_access_(subindex_access_support) {}

  bool subindexAccess() const { return subindex_access_; }

private:
  bool subindex_access_ = true;
};
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_COMPLEX_DATA_TYPE_HPP