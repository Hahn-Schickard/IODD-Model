#ifndef __IODD_STANDARD_DEFINES_ARRAY_T_HPP
#define __IODD_STANDARD_DEFINES_ARRAY_T_HPP

#include "ComplexDatatype.hpp"
#include "SimpleDataTypes.hpp"

#include <vector>

namespace IODD {

struct ArrayT : public ComplexDataTypeT {
  using Values = std::vector<SimpleDatatype>;

  ArrayT() = default;

  ArrayT(Datatype type, size_t count, Values&& values);

  ArrayT(bool subindex_access, Datatype type, size_t count, Values&& values);

  void expand(const ArrayT& other);

  size_t hash() const noexcept;

  size_t count() const;

  Values values() const;

  Datatype type() const;

private:
  Datatype type_;
  size_t count_;
  Values values_;
};

using ArrayT_Ptr = std::shared_ptr<ArrayT>;
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_ARRAY_T_HPP