#ifndef __IODD_STANDARD_DEFINES_ARRAY_T_HPP
#define __IODD_STANDARD_DEFINES_ARRAY_T_HPP

#include "ComplexDatatype.hpp"
#include "SimpleDataTypes.hpp"

#include <vector>

namespace IODD {

struct ArrayT : public ComplexDataTypeT {
  ArrayT() = default;

  ArrayT(const SimpleDatatype& type, size_t count);

  ArrayT(bool subindex_access, const SimpleDatatype& type, size_t count);

  ~ArrayT() = default;

  void expand(const ArrayT& other);

  size_t count() const;

  SimpleDatatype type() const;

private:
  SimpleDatatype type_;
  size_t count_;
};

using ArrayT_Ptr = std::shared_ptr<ArrayT>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_ARRAY_T_HPP