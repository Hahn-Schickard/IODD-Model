#ifndef __IODD_STANDARD_DEFINES_COMPLEX_DATA_TYPE_HPP
#define __IODD_STANDARD_DEFINES_COMPLEX_DATA_TYPE_HPP

#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "OctetString.hpp"
#include "String.hpp"
#include "Time.hpp"
#include "Timespan.hpp"
#include "UInteger.hpp"

#include <type_traits>

namespace IODD {
template <typename T, typename SFINAE = void> struct ComplexDataTypeT {
  ComplexDataTypeT() = default;

  ComplexDataTypeT(bool subindex_access_support)
      : subindex_access_(subindex_access_support) {}

  bool subindexAccess() const { return subindex_access_; }

private:
  bool subindex_access_ = true;
};

template <typename T>
using IsSimpleDatatype = typename std::enable_if<
    std::is_same<T, BooleanT>::value || std::is_same<T, UIntegerT>::value ||
        std::is_same<T, IntegerT>::value || std::is_same<T, FloatT>::value ||
        std::is_same<T, OctetStringT>::value ||
        std::is_same<T, StringT>::value || std::is_same<T, TimeT>::value ||
        std::is_same<T, TimeSpanT>::value,
    T>::type;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_COMPLEX_DATA_TYPE_HPP