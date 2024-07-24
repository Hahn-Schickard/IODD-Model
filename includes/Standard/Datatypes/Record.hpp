#ifndef __IODD_STANDARD_DEFINES_RECORD_T_HPP
#define __IODD_STANDARD_DEFINES_RECORD_T_HPP

#include "AccessRights.hpp"
#include "ComplexDatatype.hpp"
#include "Primitives/TextID.hpp"

#include <unordered_map>

namespace IODD {

template <typename T> struct RecordItem {
  RecordItem() = default;

  RecordItem(uint8_t subindex,
      uint16_t offset,
      T&& value,
      TextID&& name,
      std::optional<AccessRights> access = std::nullopt,
      std::optional<TextID>&& desc = std::nullopt)
      : subindex_(subindex),
        offset_(FixedBitLength<0, 1855>(offset).bitLength()),
        value_(std::move(value)), name_(std::move(name)),
        access_(std::move(access)), desc_(std::move(desc)) {}

  size_t hash() const noexcept { return subindex_; }

  uint8_t subindex() const { return subindex_; }

  uint16_t offset() const { return offset_; }

  T value() const { return value_; }

  TextID name() const { return name_; }

  std::optional<AccessRights> access() const { return access_; }

  std::optional<TextID> description() const { return desc_; }

private:
  uint8_t subindex_;
  uint16_t offset_;
  T value_;
  TextID name_;
  std::optional<AccessRights> access_;
  std::optional<TextID> desc_;
};

template <typename T>
using RecordItems = std::unordered_map<uint8_t, RecordItem<T>>;

template <typename T>
struct RecordT : public FixedBitLength<1, 1856>,
                 public ComplexDataTypeT<T, IsSimpleDatatype<T>> {
  RecordT() = default;

  RecordT(uint16_t bit_length, RecordItems<T>&& items)
      : RecordT(bit_length, false, std::move(items)) {}

  RecordT(uint16_t bit_length, bool subindex_access, RecordItems<T>&& items)
      : FixedBitLength(bit_length), // clang-format off
        ComplexDataTypeT<T, IsSimpleDatatype<T>>(subindex_access),
        items_(std::move(items)) {} // clang-format on

  void expand(const RecordT& other) {
    // existing key values should NOT be updated
    items_.insert(other.items_.begin(), other.items_.end());
  }

  size_t hash() const noexcept {
    size_t result;
    for (const auto& item : items_) {
      result += (item.second.hash() < 8) | item.second.value.hash();
    }
    return result;
  }

  RecordItems<T> items() const { return items_; }

  RecordItem<T> item(uint8_t subindex) const {
    if (const auto& it = items_.find(subindex); it != items_.end()) {
      return it->second;
    }
    throw std::out_of_range(
        "Record " + std::to_string(subindex) + " does not exits");
  }

private:
  RecordItems<T> items_;
};

template <typename T>
inline bool operator==(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex() == rhs.subindex()) && (lhs.value() == rhs.value()) &&
      (lhs.name() == rhs.name()) && (lhs.access() == rhs.access()) &&
      (lhs.description() == rhs.description());
}

template <typename T>
inline bool operator!=(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex() != rhs.subindex()) && (lhs.value() != rhs.value()) &&
      (lhs.name() != rhs.name()) && (lhs.access() != rhs.access()) &&
      (lhs.description() != rhs.description());
}

template <typename T>
inline bool operator<=(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex() <= rhs.subindex()) && (lhs.value() <= rhs.value()) &&
      (lhs.name() <= rhs.name()) && (lhs.access() <= rhs.access()) &&
      (lhs.description() <= rhs.description());
}

template <typename T>
inline bool operator>=(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex() >= rhs.subindex()) && (lhs.value() >= rhs.value()) &&
      (lhs.name() >= rhs.name()) && (lhs.access() >= rhs.access()) &&
      (lhs.description() >= rhs.description());
}

template <typename T>
inline bool operator<(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex() < rhs.subindex()) && (lhs.value() < rhs.value()) &&
      (lhs.name() < rhs.name()) && (lhs.access() < rhs.access()) &&
      (lhs.description() < rhs.description());
}

template <typename T>
inline bool operator>(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex() > rhs.subindex()) && (lhs.value() > rhs.value()) &&
      (lhs.name() > rhs.name()) && (lhs.access() > rhs.access()) &&
      (lhs.description() > rhs.description());
}
} // namespace IODD
#endif //__IODD_STANDARD_DEFINES_RECORD_T_HPP