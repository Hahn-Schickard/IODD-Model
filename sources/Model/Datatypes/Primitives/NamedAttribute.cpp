#include "NamedAttribute.hpp"

using namespace std;

namespace IODD {

NamedAttribute::NamedAttribute(const TextIDPtr& name) :
    name_(name) {}

TextIDPtr NamedAttribute::tryName() {
  if (!name_) {
    throw AttributeNotNamed();
  }
  return name_;
}

TextIDPtr NamedAttribute::name() { return name_; }
}; // namespace IODD
