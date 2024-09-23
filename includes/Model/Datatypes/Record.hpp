#ifndef __IODD_STANDARD_DEFINES_RECORD_T_HPP
#define __IODD_STANDARD_DEFINES_RECORD_T_HPP

#include "AccessRights.hpp"
#include "ComplexDatatype.hpp"
#include "Primitives/TextID.hpp"

#include <optional>
#include <unordered_map>

namespace IODD {

template <typename T> struct RecordItem {
  RecordItem() = default;

  RecordItem(uint8_t subindex,
      uint16_t offset,
      T&& value,
      TextID&& name,
      std::optional<AccessRights> access = std::nullopt,
      std::optional<TextID>&& desc = std::nullopt);

  size_t hash() const noexcept;

  uint8_t subindex() const;

  uint16_t offset() const;

  T value() const;

  TextID name() const;

  std::optional<AccessRights> access() const;

  std::optional<TextID> description() const;

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

template struct RecordItem<BooleanT>;
template struct RecordItem<UIntegerT>;
template struct RecordItem<IntegerT>;
template struct RecordItem<FloatT>;
template struct RecordItem<StringT>;
template struct RecordItem<OctetStringT>;
template struct RecordItem<TimeT>;
template struct RecordItem<TimeSpanT>;

template <typename T>
struct RecordT : public FixedBitLength<1, 1856>,
                 public ComplexDataTypeT<T, IsSimpleDatatype<T>> {
  RecordT() = default;

  RecordT(uint16_t bit_length, RecordItems<T>&& items);

  RecordT(uint16_t bit_length, bool subindex_access, RecordItems<T>&& items);

  void expand(const RecordT& other);

  size_t hash() const noexcept;

  RecordItems<T> items() const;

  RecordItem<T> item(uint8_t subindex) const;

private:
  RecordItems<T> items_;
};

template struct RecordT<BooleanT>;
template struct RecordT<UIntegerT>;
template struct RecordT<IntegerT>;
template struct RecordT<FloatT>;
template struct RecordT<StringT>;
template struct RecordT<OctetStringT>;
template struct RecordT<TimeT>;
template struct RecordT<TimeSpanT>;

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
