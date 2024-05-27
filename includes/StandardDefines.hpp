#ifndef __IODD_STANDARD_DEFINES_HPP
#define __IODD_STANDARD_DEFINES_HPP

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>

namespace IODD {

struct TextID {
  TextID(const std::string& id, const std::string& locale)
      : id_(id), locale_(locale) {}

  std::string id() const { return id_; }

  std::string locale() const { return locale_; }

  size_t hash() const noexcept { return std::hash<std::string>{}(id_); }

private:
  std::string id_;
  std::string locale_;
};

struct NamedAttribute {
  const std::optional<TextID> name = std::nullopt;

  NamedAttribute() = default;

  NamedAttribute(std::optional<TextID>&& _name) : name(std::move(_name)) {}
};

using NamedAttributePtr = std::shared_ptr<NamedAttribute>;

template <typename T> struct SingleValue : public NamedAttribute {
  const T value;

  SingleValue(T _value) : value(_value) {}

  SingleValue(T _value, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), value(_value) {}

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

  ValueRange(T _lower, T _upper, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), lower(_lower), upper(_upper) {}

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

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : values) {
      result += value->hash();
    }
    return result;
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

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : single_values) {
      result += value->hash();
    }
    for (const auto& value : value_ranges) {
      result += value->hash();
    }
    return result;
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

  size_t hash() const noexcept { return fixed_len; }
};

struct StringT : public OctetStringT {
  const bool utf_encoding; // US-ASCII if false

  StringT(size_t length, bool utf = true)
      : OctetStringT(length), utf_encoding(utf) {}

  size_t hash() const noexcept { return (fixed_len < 1) | utf_encoding; }
};

struct TimeT {
  size_t hash() const noexcept { return -1; }
};

struct TimeSpanT {
  size_t hash() const noexcept { return -2; }
};

template <typename T, typename SFINAE = void> struct ComplexDataTypeT {
  const bool subindex_access = true;

  ComplexDataTypeT() = default;

  ComplexDataTypeT(bool subindex_access_support)
      : subindex_access(subindex_access_support) {}
};

template <typename T>
using IsSimpleDatatype = typename std::enable_if<
    std::is_same<T, BooleanT>::value || std::is_same<T, UIntegerT>::value ||
        std::is_same<T, IntegerT>::value || std::is_same<T, FloatT>::value ||
        std::is_same<T, OctetStringT>::value ||
        std::is_same<T, StringT>::value || std::is_same<T, TimeT>::value ||
        std::is_same<T, TimeSpanT>::value,
    T>::type;

template <typename T>
struct ArrayT : public ComplexDataTypeT<T, IsSimpleDatatype<T>> {
  const size_t count;
  const std::vector<T> values;

  ArrayT(size_t _count, std::vector<T>&& _values)
      : ArrayT(_count, false, std::move(_values)) {}

  ArrayT(size_t _count, bool subindex_access, std::vector<T>&& _values)
      : ComplexDataTypeT<T, IsSimpleDatatype<T>>(subindex_access),
        count(_count), values(std::move(_values)) {}
};

enum class AccessRights { READ_ONLY, WRITE_ONLY, READ_WRITE };

template <typename T> struct RecordItem {
  const size_t subindex;
  const uint16_t bit_offset;
  const T value;
  const TextID name;
  const std::optional<AccessRights> access;
  const std::optional<TextID> desc;

  RecordItem(size_t _subindex, uint16_t _bit_offset, T&& _value, TextID&& _name,
      std::optional<AccessRights>&& _access = std::nullopt,
      std::optional<TextID>&& _desc = std::nullopt)
      : subindex(_subindex),
        bit_offset(FixedBitLength<0, 1855>(_bit_offset).bit_length),
        value(std::move(_value)), name(std::move(_name)),
        access(std::move(_access)), desc(std::move(_desc)) {}

  size_t hash() const noexcept { return subindex; }
};

struct Hash {
  template <typename T> size_t operator()(const RecordItem<T>& item) const {
    return (item.hash());
  }
};

template <typename T>
using RecordItems = std::unordered_set<RecordItem<T>, Hash>;

template <typename T>
struct RecordT : public FixedBitLength<1, 1856>,
                 public ComplexDataTypeT<T, IsSimpleDatatype<T>> {
  const RecordItems<T> items;

  RecordT(uint16_t bit_length, RecordItems<T>&& _items)
      : RecordT(bit_length, false, std::move(_items)) {}

  RecordT(uint16_t bit_length, bool subindex_access, RecordItems<T>&& _items)
      : FixedBitLength(bit_length), // clang-format off
        ComplexDataTypeT<T, IsSimpleDatatype<T>>(subindex_access),
        items(std::move(_items)) {} // clang-format on
};

using DataValue = std::variant< // clang-format off
        BooleanT, 
        UIntegerT, 
        IntegerT, 
        FloatT, 
        OctetStringT, 
        StringT,
        TimeT, 
        TimeSpanT, 
        ArrayT<BooleanT>, 
        ArrayT<UIntegerT>, 
        ArrayT<IntegerT>,
        ArrayT<FloatT>, 
        ArrayT<OctetStringT>, 
        ArrayT<StringT>, 
        ArrayT<TimeT>,
        ArrayT<TimeSpanT>, 
        RecordT<BooleanT>, 
        RecordT<UIntegerT>,
        RecordT<IntegerT>, 
        RecordT<FloatT>, 
        RecordT<OctetStringT>,
        RecordT<StringT>, 
        RecordT<TimeT>, 
        RecordT<TimeSpanT>
>; // clang-format on

struct Unit : public NamedAttribute {
  const uint16_t code;
  const std::string abbr;

  Unit(uint16_t _code, const std::string& _abbr) : code(_code), abbr(_abbr) {}

  Unit(uint16_t _code, const std::string& _abbr, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), code(_code), abbr(_abbr) {}

  size_t hash() const noexcept { return std::hash<uint16_t>{}(code); }
};

struct Variable {
  const size_t index;
  const TextID name;
  const AccessRights access;
  const DataValue value;
  const std::optional<TextID> desc;
  const bool dynamic;
  const bool modifies_others;
  const bool excluded;

  Variable(size_t _index, TextID&& _name, AccessRights _access,
      DataValue&& _value, std::optional<TextID>&& _desc = std::nullopt,
      bool _dynamic = false, bool _modifies_others = false,
      bool _excluded = false)
      : index(_index), name(std::move(_name)), access(_access),
        value(std::move(_value)), desc(std::move(_desc)), dynamic(_dynamic),
        modifies_others(_modifies_others), excluded(_excluded) {}
};

// Comparator functions
inline bool operator==(const TextID& lhs, const TextID& rhs) {
  return lhs.id() == rhs.id();
}

inline bool operator!=(const TextID& lhs, const TextID& rhs) {
  return lhs.id() != rhs.id();
}

inline bool operator<=(const TextID& lhs, const TextID& rhs) {
  return lhs.id() <= rhs.id();
}

inline bool operator>=(const TextID& lhs, const TextID& rhs) {
  return lhs.id() >= rhs.id();
}

inline bool operator<(const TextID& lhs, const TextID& rhs) {
  return lhs.id() < rhs.id();
}

inline bool operator>(const TextID& lhs, const TextID& rhs) {
  return lhs.id() > rhs.id();
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

inline bool operator==(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const BooleanT& lhs, const BooleanT& rhs) {
  return lhs.hash() > rhs.hash();
}

template <typename T>
inline bool operator==(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() == rhs.hash();
}

template <typename T>
inline bool operator!=(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() != rhs.hash();
}

template <typename T>
inline bool operator<=(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() <= rhs.hash();
}

template <typename T>
inline bool operator>=(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() >= rhs.hash();
}

template <typename T>
inline bool operator<(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() < rhs.hash();
}

template <typename T>
inline bool operator>(const NumberT<T>& lhs, const NumberT<T>& rhs) {
  return lhs.hash() > rhs.hash();
}

inline bool operator==(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const OctetStringT& lhs, const OctetStringT& rhs) {
  return lhs.hash() > rhs.hash();
}

inline bool operator==(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const TimeT& lhs, const TimeT& rhs) {
  return lhs.hash() > rhs.hash();
}

inline bool operator==(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() == rhs.hash();
}

inline bool operator!=(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() != rhs.hash();
}

inline bool operator<=(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() <= rhs.hash();
}

inline bool operator>=(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() >= rhs.hash();
}

inline bool operator<(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const TimeSpanT& lhs, const TimeSpanT& rhs) {
  return lhs.hash() > rhs.hash();
}

template <typename T>
inline bool operator==(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex == rhs.subindex) && (lhs.value == rhs.value) &&
      (lhs.name == rhs.name) && (lhs.access == rhs.access) &&
      (lhs.desc == rhs.desc);
}

template <typename T>
inline bool operator!=(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex != rhs.subindex) && (lhs.value != rhs.value) &&
      (lhs.name != rhs.name) && (lhs.access != rhs.access) &&
      (lhs.desc != rhs.desc);
}

template <typename T>
inline bool operator<=(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex <= rhs.subindex) && (lhs.value <= rhs.value) &&
      (lhs.name <= rhs.name) && (lhs.access <= rhs.access) &&
      (lhs.desc <= rhs.desc);
}

template <typename T>
inline bool operator>=(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex >= rhs.subindex) && (lhs.value >= rhs.value) &&
      (lhs.name >= rhs.name) && (lhs.access >= rhs.access) &&
      (lhs.desc >= rhs.desc);
}

template <typename T>
inline bool operator<(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex < rhs.subindex) && (lhs.value < rhs.value) &&
      (lhs.name < rhs.name) && (lhs.access < rhs.access) &&
      (lhs.desc < rhs.desc);
}

template <typename T>
inline bool operator>(const RecordItem<T>& lhs, const RecordItem<T>& rhs) {
  return (lhs.subindex > rhs.subindex) && (lhs.value > rhs.value) &&
      (lhs.name > rhs.name) && (lhs.access > rhs.access) &&
      (lhs.desc > rhs.desc);
}

inline bool operator==(const Unit& lhs, const Unit& rhs) {
  return lhs.code == rhs.code;
}

inline bool operator!=(const Unit& lhs, const Unit& rhs) {
  return lhs.code != rhs.code;
}

inline bool operator<=(const Unit& lhs, const Unit& rhs) {
  return lhs.code <= rhs.code;
}

inline bool operator>=(const Unit& lhs, const Unit& rhs) {
  return lhs.code >= rhs.code;
}

inline bool operator<(const Unit& lhs, const Unit& rhs) {
  return lhs.code < rhs.code;
}

inline bool operator>(const Unit& lhs, const Unit& rhs) {
  return lhs.code > rhs.code;
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

template <> struct std::hash<IODD::Unit> {
  std::size_t operator()(const IODD::Unit& object) const noexcept {
    return object.hash();
  }
};
#endif //__IODD_STANDARD_DEFINES_HPP