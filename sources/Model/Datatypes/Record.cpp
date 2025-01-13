#include "Record.hpp"

#include <Variant_Visitor.hpp>
#include <stdexcept>

using namespace std;

namespace IODD {

RecordItem::RecordItem(uint8_t subindex,
    uint16_t offset,
    SimpleDatatype&& value,
    TextID&& name,
    optional<AccessRights> access,
    optional<TextID>&& desc)
    : subindex_(subindex),
      // NOLINTNEXTLINE(readability-magic-numbers)
      offset_(FixedBitLength<0, 1855>(offset).bitLength()), value_(move(value)),
      name_(move(name)), access_(access), desc_(move(desc)) {}

size_t RecordItem::hash() const noexcept { return subindex_; }

uint8_t RecordItem::subindex() const { return subindex_; }

uint16_t RecordItem::offset() const { return offset_; }

SimpleDatatype RecordItem::value() const { return value_; }

TextID RecordItem::name() const { return name_; }

optional<AccessRights> RecordItem::access() const { return access_; }

optional<TextID> RecordItem::description() const { return desc_; }

RecordT::RecordT(uint16_t bit_length, RecordItems&& items)
    : RecordT(bit_length, false, move(items)) {}

RecordT::RecordT(uint16_t bit_length, bool subindex_access, RecordItems&& items)
    : FixedBitLength(bit_length), // clang-format off
        ComplexDataTypeT(subindex_access),
        items_(move(items)) {} // clang-format on

void RecordT::expand(const RecordT& other) {
  // existing key values should NOT be updated
  items_.insert(other.items_.begin(), other.items_.end());
}

size_t RecordT::hash() const noexcept {
  size_t result = 0;
  for (const auto& item : items_) {
    result += item.second->hash();
  }
  return result;
}

RecordItems RecordT::items() const { return items_; }

RecordItem_Ptr RecordT::item(uint8_t subindex) const {
  if (const auto& it = items_.find(subindex); it != items_.end()) {
    return it->second;
  }
  throw out_of_range("Record " + to_string(subindex) + " does not exits");
}
} // namespace IODD
