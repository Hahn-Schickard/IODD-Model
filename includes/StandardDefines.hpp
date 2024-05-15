#ifndef __IODD_STANDARD_DEFINES_HPP
#define __IODD_STANDARD_DEFINES_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace IODD {

struct TextID {
  const std::string id;
  const std::string locale;

  TextID(const std::string& _id, const std::string& _locale)
      : id(_id), locale(_locale) {}

  size_t hash() const noexcept { return std::hash<std::string>{}(id); }
};

struct NamedAttribute {
  const std::optional<TextID> name = std::nullopt;

  NamedAttribute() = default;

  NamedAttribute(const TextID& _name) : name(_name) {}

  template <typename... Args>
  NamedAttribute(Args&&... args) : name(TextID(std::forward(args...))) {}
};

using NamedAttributePtr = std::shared_ptr<NamedAttribute>;

template <typename T> struct SingleValue : public NamedAttribute {
  const T value;

  SingleValue(T _value) : value(_value) {}

  size_t hash() const noexcept { return std::hash<T>{}(value); }
};

template <typename T> using SingleValuePtr = std::shared_ptr<SingleValue<T>>;

template <typename T> struct ValueRange : public NamedAttribute {
  const T lower;
  const T upper;

  ValueRange(T _lower, T _upper) : lower(_lower), upper(_upper) {
    if (upper <= lower) {
      throw std::invalid_argument(
          "Upper bound must be larger than lower bound");
    }
  }

  size_t hash() const noexcept {
    return std::hash<T>{}(lower) + std::hash<T>{}(upper);
  }
};

template <typename T> using ValueRangePtr = std::shared_ptr<ValueRange<T>>;

struct BooleanT {
  const std::unordered_set<SingleValuePtr<bool>> values;

  BooleanT() = default;

  BooleanT(std::unordered_set<SingleValuePtr<bool>>&& _values)
      : values(std::move(_values)) {}
};

template <typename T> struct NumberT {
  using SingleValues = std::unordered_set<SingleValuePtr<T>>;
  using ValueRanges = std::unordered_set<ValueRangePtr<T>>;

  const std::optional<SingleValues> single_values = std::nullopt;
  const std::optional<ValueRanges> value_ranges = std::nullopt;

  NumberT() = default;

  NumberT(SingleValues&& _values) : single_values(std::move(_values)) {}

  NumberT(ValueRanges&& _values) : value_ranges(std::move(_values)) {}

  NumberT(SingleValues&& _single_values, ValueRanges&& _value_ranges)
      : single_values(std::move(_single_values)),
        value_ranges(std::move(_value_ranges)) {}
};

template <size_t MIN, size_t MAX> struct FixedBitLength {
  const size_t bit_length; // or bit_offset

  FixedBitLength(uint8_t bits) : bit_length(bits) {
    if (bit_length < MIN) {
      throw std::invalid_argument(
          "Bit length can not be smaller than " + std::to_string(MIN));
    } else if (bit_length > MAX) {
      throw std::invalid_argument(
          "Bit length can not be larger than " + std::to_string(MAX));
    }
  }
};

struct UIntegerT : public NumberT<size_t>, public FixedBitLength<2, 64> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  UIntegerT(uint8_t bits, SingleValues&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  UIntegerT(uint8_t bits, ValueRanges&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  UIntegerT(
      uint8_t bits, SingleValues&& single_values, ValueRanges&& value_ranges)
      : FixedBitLength(bits),
        NumberT(std::move(single_values), std::move(value_ranges)) {}
};

struct IntegerT : public NumberT<intmax_t>, public FixedBitLength<2, 64> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  IntegerT(uint8_t bits, SingleValues&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  IntegerT(uint8_t bits, ValueRanges&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  IntegerT(
      uint8_t bits, SingleValues&& single_values, ValueRanges&& value_ranges)
      : FixedBitLength(bits),
        NumberT(std::move(single_values), std::move(value_ranges)) {}
};

struct FloatT : public NumberT<float> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  FloatT(SingleValues&& values) : NumberT(std::move(values)) {}

  FloatT(ValueRanges&& values) : NumberT(std::move(values)) {}

  FloatT(SingleValues&& single_values, ValueRanges&& value_ranges)
      : NumberT(std::move(single_values), std::move(value_ranges)) {}
};

struct OctetStringT {
  const size_t fixed_len;

  OctetStringT(size_t length) : fixed_len(length) {}
};

struct StringT : public OctetStringT {
  const bool utf_encoding = true; // US-ASCII if false

  StringT(size_t length, bool utf = true)
      : OctetStringT(length), utf_encoding(utf) {}
};

struct TimeT {};

struct TimeSpanT {};

using SimpleDatatype = std::variant<BooleanT, UIntegerT, IntegerT, FloatT,
    OctetStringT, StringT, TimeT, TimeSpanT>;

struct ComplexDataTypeT {
  const bool subindex_access = true;

  ComplexDataTypeT() = default;

  ComplexDataTypeT(bool subindex_access_support)
      : subindex_access(subindex_access_support) {}
};

struct ArrayT : public ComplexDataTypeT {
  const std::vector<SimpleDatatype> values; // does it allow data type mixing?

  ArrayT(size_t count, std::vector<SimpleDatatype>&& _values,
      bool subindex_access = true)
      : values(std::move(values)), ComplexDataTypeT(subindex_access) {
    if (values.size() != count) {
      throw std::invalid_argument("Provided vector of size " +
          std::to_string(values.size()) +
          " does not mach the specified array size of " +
          std::to_string(count));
    }
  }
};

enum class AccessRights { READ_ONLY, WRITE_ONLY, READ_WRITE };

struct RecordItem {
  const size_t subindex;
  const uint16_t bit_offset;
  const SimpleDatatype type;
  const TextID name;
  const std::optional<AccessRights> access;
  const std::optional<TextID> desc;

  RecordItem(size_t _subindex, uint16_t _bit_offset, SimpleDatatype&& _type,
      TextID&& _name, std::optional<AccessRights> _access = std::nullopt,
      std::optional<TextID> _desc = std::nullopt)
      : subindex(subindex),
        bit_offset(FixedBitLength<0, 1855>(_bit_offset).bit_length),
        type(std::move(_type)), name(std::move(_name)), access(_access),
        desc(_desc) {}

  template <typename... Args>
  RecordItem(size_t _subindex, uint16_t _bit_offset, SimpleDatatype&& _type,
      Args&&... args)
      : subindex(subindex),
        bit_offset(FixedBitLength<0, 1855>(_bit_offset).bit_length),
        type(std::move(_type)), name(std::forward(args...)) {}

  template <typename... Args>
  RecordItem(size_t _subindex, uint16_t _bit_offset, SimpleDatatype&& _type,
      std::optional<AccessRights> _access, std::optional<TextID> _desc,
      Args&&... args)
      : subindex(subindex),
        bit_offset(FixedBitLength<0, 1855>(_bit_offset).bit_length),
        type(std::move(_type)), name(std::forward(args...)), access(_access),
        desc(_desc) {}

  size_t hash() const noexcept { return subindex; }
};

struct Hash {
  size_t operator()(const RecordItem& item) const { return (item.hash()); }
};

struct RecordT : public FixedBitLength<1, 1856>, public ComplexDataTypeT {
  using Records = std::unordered_set<RecordItem, Hash>;
  const Records items;

  RecordT(uint16_t bit_length, Records&& _items, bool subindex_access = true)
      : FixedBitLength(bit_length), items(std::move(_items)),
        ComplexDataTypeT(subindex_access) {}
};

struct Datatype {
  using Type = std::variant<BooleanT, UIntegerT, IntegerT, FloatT, OctetStringT,
      StringT, TimeT, TimeSpanT, ArrayT, RecordT>;
  const std::string id;
  const Type type;

  Datatype(const std::string& _id, Type&& _type)
      : id(_id), type(std::move(_type)) {}

  size_t hash() const noexcept { return std::hash<std::string>{}(id); }
};

using DatatypeCollection = std::unordered_set<Datatype>;

struct StdSingleValueRef {};

template <typename T> struct VariableT {
  using Variant =
      std::variant<StdSingleValueRef, SingleValue<T>, ValueRange<T>>;

  const std::string id;
  const uint16_t index;
  const AccessRights access;
  const bool dynamic = false;
  const bool modifies_others = false;
  const bool excluded_from_storage = false;
  const std::optional<SimpleDatatype>
      default_value; // is anySimpleType the same as SimpleDatatype?
};

// Comparator functions
inline bool operator==(const TextID& lhs, const TextID& rhs) {
  return lhs.id == rhs.id;
}

inline bool operator!=(const TextID& lhs, const TextID& rhs) {
  return lhs.id != rhs.id;
}

inline bool operator<=(const TextID& lhs, const TextID& rhs) {
  return lhs.id <= rhs.id;
}

inline bool operator>=(const TextID& lhs, const TextID& rhs) {
  return lhs.id >= rhs.id;
}

inline bool operator<(const TextID& lhs, const TextID& rhs) {
  return lhs.id < rhs.id;
}

inline bool operator>(const TextID& lhs, const TextID& rhs) {
  return lhs.id > rhs.id;
}

template <typename T>
inline bool operator==(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value == rhs.value;
}

template <typename T>
inline bool operator!=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value != rhs.value;
}

template <typename T>
inline bool operator<=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value <= rhs.value;
}

template <typename T>
inline bool operator>=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value >= rhs.value;
}

template <typename T>
inline bool operator<(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value < rhs.value;
}

template <typename T>
inline bool operator>(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value > rhs.value;
}

template <typename T>
inline bool operator==(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower == rhs.lower) && (lhs.upper == rhs.upper);
}

template <typename T>
inline bool operator!=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower != rhs.lower) && (lhs.upper != rhs.upper);
}

template <typename T>
inline bool operator<=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower <= rhs.lower) && (lhs.upper <= rhs.upper);
}

template <typename T>
inline bool operator>=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower >= rhs.lower) && (lhs.upper >= rhs.upper);
}

template <typename T>
inline bool operator<(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower < rhs.lower) && (lhs.upper < rhs.upper);
}

template <typename T>
inline bool operator>(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower > rhs.lower) && (lhs.upper > rhs.upper);
}

inline bool operator==(const RecordItem& lhs, const RecordItem& rhs) {
  return (lhs.subindex == rhs.subindex) &&
      (lhs.type.index() == rhs.type.index()) && (lhs.name == rhs.name) &&
      (lhs.access == rhs.access) && (lhs.desc == rhs.desc);
}

inline bool operator!=(const RecordItem& lhs, const RecordItem& rhs) {
  return (lhs.subindex != rhs.subindex) &&
      (lhs.type.index() != rhs.type.index()) && (lhs.name != rhs.name) &&
      (lhs.access != rhs.access) && (lhs.desc != rhs.desc);
}

inline bool operator<=(const RecordItem& lhs, const RecordItem& rhs) {
  return (lhs.subindex <= rhs.subindex) &&
      (lhs.type.index() <= rhs.type.index()) && (lhs.name <= rhs.name) &&
      (lhs.access <= rhs.access) && (lhs.desc <= rhs.desc);
}

inline bool operator>=(const RecordItem& lhs, const RecordItem& rhs) {
  return (lhs.subindex >= rhs.subindex) &&
      (lhs.type.index() >= rhs.type.index()) && (lhs.name >= rhs.name) &&
      (lhs.access >= rhs.access) && (lhs.desc >= rhs.desc);
}

inline bool operator<(const RecordItem& lhs, const RecordItem& rhs) {
  return (lhs.subindex < rhs.subindex) &&
      (lhs.type.index() < rhs.type.index()) && (lhs.name < rhs.name) &&
      (lhs.access < rhs.access) && (lhs.desc < rhs.desc);
}

inline bool operator>(const RecordItem& lhs, const RecordItem& rhs) {
  return (lhs.subindex > rhs.subindex) &&
      (lhs.type.index() > rhs.type.index()) && (lhs.name > rhs.name) &&
      (lhs.access > rhs.access) && (lhs.desc > rhs.desc);
}

} // namespace IODD

// Hashing functions
template <> struct std::hash<IODD::TextID> {
  std::size_t operator()(const IODD::TextID& object) const noexcept {
    return object.hash();
  }
};

template <typename T> struct std::hash<IODD::SingleValue<T>> {
  std::size_t operator()(const IODD::SingleValue<T>& object) const noexcept {
    return object.hash();
  }
};

template <typename T> struct std::hash<IODD::ValueRange<T>> {
  std::size_t operator()(const IODD::ValueRange<T>& object) const noexcept {
    return object.hash();
  }
};
#endif //__IODD_STANDARD_DEFINES_HPP