#include "ProcessData.hpp"

using namespace std;

namespace IODD {

ProcessDataT::ProcessDataT(
    const string& id, uint16_t bits, const TextIDPtr& name, DataValue&& value) :
    FixedBitLength(bits),
    id_(id),
    name_(name),
    value_(move(value)) {
  if (!name) {
    throw invalid_argument("Process Data Type must have a valid name");
  }
}

string ProcessDataT::id() const { return id_; }

TextIDPtr ProcessDataT::name() const { return name_; }

DataValue ProcessDataT::value() const { return value_; }

NamedAttributePtr ProcessDataT::valueName(
    const SimpleDatatypeValue& value, optional<uint8_t> subindex) const {
  return getValueName(value_, value, subindex);
}

Datatype ProcessDataT::type() const { return toDatatype(value_); }
} // namespace IODD
