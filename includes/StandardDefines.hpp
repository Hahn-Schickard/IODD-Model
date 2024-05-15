#ifndef __IODD_STANDARD_DEFINES_HPP
#define __IODD_STANDARD_DEFINES_HPP

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
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

  SingleValue(T _value, const TextID& name)
      : value(_value), NamedAttribute(name) {}

  template <typename... Args>
  SingleValue(T _value, Args&&... args)
      : value(_value), NamedAttribute(std::forward(args...)) {}

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

  ValueRange(T _lower, T _upper, const TextID& name)
      : lower(_lower), upper(_upper), NamedAttribute(name) {}

  template <typename... Args>
  ValueRange(T _lower, T _upper, Args&&... args)
      : lower(_lower), upper(_upper), NamedAttribute(std::forward(args...)) {}

  bool inRange(T value) const noexcept {
    return (value > lower) && (value < upper);
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

  NamedAttributePtr getName(bool value) const {
    if (auto it = values.find(std::make_shared<SingleValue<bool>>(value));
        it != values.end()) {
      return *it;
    }
    throw std::out_of_range(
        std::string(value ? "True" : "False") + " value has no assigned named");
  }
};

template <typename T> struct NumberT {
  using SingleValues = std::unordered_set<SingleValuePtr<T>>;
  using ValueRanges = std::unordered_set<ValueRangePtr<T>>;

  const SingleValues single_values;
  const ValueRanges value_ranges;

  NumberT() = default;

  NumberT(SingleValues&& _values) : single_values(std::move(_values)) {}

  NumberT(ValueRanges&& _values) : value_ranges(std::move(_values)) {}

  NumberT(SingleValues&& _single_values, ValueRanges&& _value_ranges)
      : single_values(std::move(_single_values)),
        value_ranges(std::move(_value_ranges)) {}

  NamedAttributePtr getName(T value) {
    if (auto it = single_values.find(std::make_shared<SingleValue<T>>(value));
        it != single_values.end()) {
      return *it;
    } else {
      for (auto it : value_ranges) {
        if (it->inRange(value)) {
          return *it;
        }
      }
    }
    throw std::out_of_range(
        std::to_string(value) + " value has no assigned named");
  }
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

struct UIntegerT : public FixedBitLength<2, 64>, public NumberT<size_t> {
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

struct IntegerT : public FixedBitLength<2, 64>, public NumberT<intmax_t> {
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
  const bool utf_encoding; // US-ASCII if false

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

using DataValue = std::variant<BooleanT, UIntegerT, IntegerT, FloatT,
    OctetStringT, StringT, TimeT, TimeSpanT, ArrayT, RecordT>;

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