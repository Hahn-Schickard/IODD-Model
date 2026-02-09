#include "Array.hpp"

using namespace std;

namespace IODD {

ArrayT::ArrayT(const SimpleDatatype& type, size_t count) :
    ArrayT(false, type, count) {}

ArrayT::ArrayT(bool subindex_access, const SimpleDatatype& type, size_t count) :
    ComplexDataTypeT(subindex_access),
    type_(type),
    count_(count) {}

void ArrayT::expand(const ArrayT& other) { IODD::expand(type_, other.type_); }

size_t ArrayT::count() const { return count_; }

SimpleDatatype ArrayT::type() const { return type_; }
} // namespace IODD
