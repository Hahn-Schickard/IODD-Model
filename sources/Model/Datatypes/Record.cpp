#include "Datatypes/Record.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

template <typename T>
RecordItem<T>::RecordItem(uint8_t subindex,
    uint16_t offset,
    T&& value,
    TextID&& name,
    optional<AccessRights> access,
    optional<TextID>&& desc)
    : subindex_(subindex),
      // NOLINTNEXTLINE(readability-magic-numbers)
      offset_(FixedBitLength<0, 1855>(offset).bitLength()), value_(move(value)),
      name_(move(name)), access_(access), desc_(move(desc)) {}

template <typename T> size_t RecordItem<T>::hash() const noexcept {
  return subindex_;
}

template <typename T> uint8_t RecordItem<T>::subindex() const {
  return subindex_;
}

template <typename T> uint16_t RecordItem<T>::offset() const { return offset_; }

template <typename T> T RecordItem<T>::value() const { return value_; }

template <typename T> TextID RecordItem<T>::name() const { return name_; }

template <typename T> optional<AccessRights> RecordItem<T>::access() const {
  return access_;
}

template <typename T> optional<TextID> RecordItem<T>::description() const {
  return desc_;
}

template struct RecordItem<BooleanT>;
template struct RecordItem<UIntegerT>;
template struct RecordItem<IntegerT>;
template struct RecordItem<FloatT>;
template struct RecordItem<StringT>;
template struct RecordItem<OctetStringT>;
template struct RecordItem<TimeT>;
template struct RecordItem<TimeSpanT>;

template <typename T>
RecordT<T>::RecordT(uint16_t bit_length, RecordItems<T>&& items)
    : RecordT(bit_length, false, move(items)) {}

template <typename T>
RecordT<T>::RecordT(
    uint16_t bit_length, bool subindex_access, RecordItems<T>&& items)
    : FixedBitLength(bit_length), // clang-format off
        ComplexDataTypeT<T, IsSimpleDatatype<T>>(subindex_access),
        items_(move(items)) {} // clang-format on

template <typename T> void RecordT<T>::expand(const RecordT& other) {
  // existing key values should NOT be updated
  items_.insert(other.items_.begin(), other.items_.end());
}

template <typename T> size_t RecordT<T>::hash() const noexcept {
  size_t result = 0;
  for (const auto& item : items_) {
    // NOLINTNEXTLINE(readability-magic-numbers)
    result += (item.second.hash() << 8) | item.second.value().hash();
  }
  return result;
}

template <typename T> RecordItems<T> RecordT<T>::items() const {
  return items_;
}

template <typename T> RecordItem<T> RecordT<T>::item(uint8_t subindex) const {
  if (const auto& it = items_.find(subindex); it != items_.end()) {
    return it->second;
  }
  throw out_of_range("Record " + to_string(subindex) + " does not exits");
}

template struct RecordT<BooleanT>;
template struct RecordT<UIntegerT>;
template struct RecordT<IntegerT>;
template struct RecordT<FloatT>;
template struct RecordT<StringT>;
template struct RecordT<OctetStringT>;
template struct RecordT<TimeT>;
template struct RecordT<TimeSpanT>;
} // namespace IODD
