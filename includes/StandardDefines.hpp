#ifndef __IODD_STANDARD_DEFINES_HPP
#define __IODD_STANDARD_DEFINES_HPP

#include "Variant_Visitor.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace IODD {

struct TextID {
  TextID(const std::string& id, const std::string& locale)
      : id_(id), locale_(locale) {
    if (id_.empty()) {
      throw std::invalid_argument(
          "Failed to create TextID. ID argument can not be empty");
    }
    if (locale_.empty()) {
      throw std::invalid_argument(
          "Failed to create TextID. locale argument can not be empty");
    }
  }

  std::string id() const { return id_; }

  std::string locale() const { return locale_; }

  size_t hash() const noexcept { return std::hash<std::string>{}(id_); }

  explicit operator bool() const { return !id_.empty() && locale_.empty(); }

private:
  std::string id_;
  std::string locale_;
};

struct NamedAttribute {
  NamedAttribute() = default;

  NamedAttribute(std::optional<TextID>&& name) : name_(std::move(name)) {}

  std::optional<TextID> name() { return name_; }

private:
  std::optional<TextID> name_ = std::nullopt;
};

using NamedAttributePtr = std::shared_ptr<NamedAttribute>;

template <typename T> struct SingleValue : public NamedAttribute {
  SingleValue() = default;

  SingleValue(T value) : value_(value) {}

  SingleValue(T value, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), value_(value) {}

  size_t hash() const noexcept { return std::hash<T>{}(value_); }

  T value() const { return value_; }

private:
  T value_;
};

template <typename T> using SingleValuePtr = std::shared_ptr<SingleValue<T>>;

template <typename T> struct ValueRange : public NamedAttribute {
  ValueRange() = default;

  ValueRange(T lower, T upper) : lower_(lower), upper_(upper) {
    if (upper_ <= lower_) {
      throw std::invalid_argument(
          "Upper bound must be larger than lower bound");
    }
  }

  ValueRange(T lower, T upper, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), lower_(lower), upper_(upper) {}

  bool inRange(T value) const noexcept {
    return (value > lower_) && (value < upper_);
  }

  size_t hash() const noexcept {
    return std::hash<T>{}(lower_) + std::hash<T>{}(upper_);
  }

  T lower() const { return lower_; }

  T upper() const { return upper_; }

private:
  T lower_;
  T upper_;
};

template <typename T> using ValueRangePtr = std::shared_ptr<ValueRange<T>>;

struct BooleanT {
  using Values = std::unordered_set<SingleValuePtr<bool>>;

  BooleanT() = default;

  BooleanT(std::unordered_set<SingleValuePtr<bool>>&& values)
      : values_(std::move(values)) {}

  void expand(const BooleanT& other) {
    for (const auto& value : other.values_) {
      if (values_.find(value) == values_.end()) {
        values_.insert(value);
      }
    }
  }

  NamedAttributePtr getName(bool value) const {
    if (auto it = values_.find(std::make_shared<SingleValue<bool>>(value));
        it != values_.end()) {
      return *it;
    }
    throw std::out_of_range(
        std::string(value ? "True" : "False") + " value has no assigned named");
  }

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : values_) {
      result += value->hash();
    }
    return result;
  }

  Values values() const { return values_; }

private:
  Values values_;
};

template <typename T> struct NumberT {
  using SingleValues = std::unordered_set<SingleValuePtr<T>>;
  using ValueRanges = std::unordered_set<ValueRangePtr<T>>;

  NumberT() = default;

  NumberT(SingleValues&& values) : values_(std::move(values)) {}

  NumberT(ValueRanges&& ranges) : ranges_(std::move(ranges)) {}

  NumberT(SingleValues&& values, ValueRanges&& ranges)
      : values_(std::move(values)), ranges_(std::move(ranges)) {}

  NamedAttributePtr getName(T value) {
    if (auto it = values_.find(std::make_shared<SingleValue<T>>(value));
        it != values_.end()) {
      return *it;
    } else {
      for (const auto& range : ranges_) {
        if (range->inRange(value)) {
          return range;
        }
      }
    }
    throw std::out_of_range(
        std::to_string(value) + " value has no assigned named");
  }

  void expand(const NumberT& other) {
    for (const auto& value : other.values_) {
      if (values_.find(value) == values_.end()) {
        values_.insert(value);
      }
    }
    for (const auto& range : other.ranges_) {
      if (ranges_.find(range) == ranges_.end()) {
        ranges_.insert(range);
      }
    }
  }

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : values_) {
      result += value->hash();
    }
    for (const auto& value : ranges_) {
      result += value->hash();
    }
    return result;
  }

private:
  SingleValues values_;
  ValueRanges ranges_;
};

template <size_t MIN, size_t MAX> struct FixedBitLength {
  FixedBitLength() = default;

  FixedBitLength(uint8_t bits) : bit_length_(bits) {
    if (bit_length_ < MIN) {
      throw std::invalid_argument(
          "Bit length can not be smaller than " + std::to_string(MIN));
    } else if (bit_length_ > MAX) {
      throw std::invalid_argument(
          "Bit length can not be larger than " + std::to_string(MAX));
    }
  }

  size_t bitLength() { return bit_length_; }

private:
  size_t bit_length_;
};

struct UIntegerT : public FixedBitLength<2, 64>, public NumberT<size_t> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  UIntegerT() = default;

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

  IntegerT() = default;

  IntegerT(uint8_t bits, SingleValues&& values)
      : FixedBitLength(bits), NumberT(std::move(values)) {}

  IntegerT(uint8_t bits, ValueRanges&& ranges)
      : FixedBitLength(bits), NumberT(std::move(ranges)) {}

  IntegerT(uint8_t bits, SingleValues&& values, ValueRanges&& ranges)
      : FixedBitLength(bits), NumberT(std::move(values), std::move(ranges)) {}
};

struct FloatT : public NumberT<float> {
  using NumberT::SingleValues;
  using NumberT::ValueRanges;

  FloatT() = default;

  FloatT(SingleValues&& values) : NumberT(std::move(values)) {}

  FloatT(ValueRanges&& ranges) : NumberT(std::move(ranges)) {}

  FloatT(SingleValues&& values, ValueRanges&& ranges)
      : NumberT(std::move(values), std::move(ranges)) {}
};

struct OctetStringT {
  OctetStringT() = default;

  OctetStringT(size_t length) : length_(length) {}

  size_t hash() const noexcept { return length_; }

  size_t length() const { return length_; }

protected:
  size_t length_;
};

struct StringT : public OctetStringT {
  StringT() = default;

  StringT(size_t length, bool utf = true) : OctetStringT(length), utf_(utf) {}

  size_t hash() const noexcept { return (length_ < 1) | utf_; }

  bool utf() const { return utf_; }

private:
  bool utf_; // US-ASCII if false
};

struct TimeT {
  size_t hash() const noexcept { return -1; }
};

struct TimeSpanT {
  size_t hash() const noexcept { return -2; }
};

template <typename T, typename SFINAE = void> struct ComplexDataTypeT {
  ComplexDataTypeT() = default;

  ComplexDataTypeT(bool subindex_access_support)
      : subindex_access_(subindex_access_support) {}

  bool subindexAccess() const { return subindex_access_; }

private:
  bool subindex_access_ = true;
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
  ArrayT() = default;

  ArrayT(size_t count, std::vector<T>&& values)
      : ArrayT(count, false, std::move(values)) {}

  ArrayT(size_t count, bool subindex_access, std::vector<T>&& values)
      : ComplexDataTypeT<T, IsSimpleDatatype<T>>(subindex_access),
        count_(count), values_(std::move(values)) {}

  void expand(const ArrayT& other) {
    values_.insert(values_.end(), other.values_.begin(), other.values_.end());
  }

  size_t hash() const noexcept {
    size_t result;
    for (const auto& value : values_) {
      result += value.hash();
    }
    return result;
  }

  size_t count() const { return count_; }

  std::vector<T> values() const { return values_; }

private:
  size_t count_;
  std::vector<T> values_;
};

enum class AccessRights { READ_ONLY, WRITE_ONLY, READ_WRITE };

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

struct Hash {
  size_t operator()(const BooleanT& boolean) const { return (boolean.hash()); }

  template <typename T> size_t operator()(const NumberT<T>& number) const {
    return (number.hash());
  }

  size_t operator()(const OctetStringT& str) const { return (str.hash()); }

  size_t operator()(const StringT& str) const { return (str.hash()); }

  size_t operator()(const TimeT& time) const { return (time.hash()); }

  size_t operator()(const TimeSpanT& time_span) const {
    return (time_span.hash());
  }

  template <typename T> size_t operator()(const ArrayT<T>& array) const {
    return (array.hash());
  }

  template <typename T> size_t operator()(const RecordItem<T>& item) const {
    return (item.hash());
  }
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

using SimpleDatatype = std::variant<BooleanT,
    UIntegerT,
    IntegerT,
    FloatT,
    OctetStringT,
    StringT,
    TimeT,
    TimeSpanT>;

enum class Datatype {
  Boolean,
  UInteger,
  Integer,
  Float32,
  String,
  OctetString,
  Time,
  TimeSpan,
  Array,
  Record,
  ProcessDataIn,
  ProcessDataOut
};

inline std::string toString(Datatype type) {
  switch (type) {
  case Datatype::Boolean: {
    return "Boolean";
  }
  case Datatype::UInteger: {
    return "UInteger";
  }
  case Datatype::Integer: {
    return "Integer";
  }
  case Datatype::Float32: {
    return "Float32";
  }
  case Datatype::String: {
    return "String";
  }
  case Datatype::OctetString: {
    return "OctetString";
  }
  case Datatype::Time: {
    return "Time";
  }
  case Datatype::TimeSpan: {
    return "TimeSpan";
  }
  case Datatype::Array: {
    return "Array";
  }
  case Datatype::Record: {
    return "Record";
  }
  case Datatype::ProcessDataIn: {
    return "ProcessDataIn";
  }
  case Datatype::ProcessDataOut: {
    return "ProcessDataOut";
  }
  default: {
    throw std::runtime_error("Unhandled Datatype enum value");
  }
  }
}

inline Datatype toDatatype(const std::string& value) {
  const std::unordered_map<std::string, Datatype> data_types = {
      {"BooleanT", Datatype::Boolean},
      {"UIntegerT", Datatype::UInteger},
      {"IntegerT", Datatype::Integer},
      {"Float32T", Datatype::Float32},
      {"StringT", Datatype::String},
      {"OctetStringT", Datatype::OctetString},
      {"TimeT", Datatype::Time},
      {"TimeSpanT", Datatype::TimeSpan},
      {"ArrayT", Datatype::Array},
      {"RecordT", Datatype::Record},
      {"ProcessDataInUnionT", Datatype::ProcessDataIn},
      {"ProcessDataOutUnionT", Datatype::ProcessDataOut}};

  if (auto it = data_types.find(value); it != data_types.end()) {
    return it->second;
  }
  throw std::invalid_argument("String value: " + value +
      " can not be converted into IODD::Datatype enumeration");
}

inline Datatype toDatatype(const SimpleDatatype& variant) {
  Datatype result;
  match(
      variant,
      [&](const BooleanT&) { result = Datatype::Boolean; },
      [&](const UIntegerT&) { result = Datatype::UInteger; },
      [&](const IntegerT&) { result = Datatype::Integer; },
      [&](const FloatT&) { result = Datatype::Float32; },
      [&](const OctetStringT&) { result = Datatype::OctetString; },
      [&](const StringT&) { result = Datatype::String; },
      [&](const TimeT&) { result = Datatype::Time; },
      [&](const TimeSpanT&) { result = Datatype::TimeSpan; });
  return result;
}

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

inline Datatype toDatatype(const DataValue& variant) {
  Datatype result;
  match(
      variant,
      [&](const BooleanT&) { result = Datatype::Boolean; },
      [&](const UIntegerT&) { result = Datatype::UInteger; },
      [&](const IntegerT&) { result = Datatype::Integer; },
      [&](const FloatT&) { result = Datatype::Float32; },
      [&](const OctetStringT&) { result = Datatype::OctetString; },
      [&](const StringT&) { result = Datatype::String; },
      [&](const TimeT&) { result = Datatype::Time; },
      [&](const TimeSpanT&) { result = Datatype::TimeSpan; },
      // array types
      [&](const ArrayT<BooleanT>&) { result = Datatype::Array; },
      [&](const ArrayT<UIntegerT>&) { result = Datatype::Array; },
      [&](const ArrayT<IntegerT>&) { result = Datatype::Array; },
      [&](const ArrayT<FloatT>&) { result = Datatype::Array; },
      [&](const ArrayT<OctetStringT>&) { result = Datatype::Array; },
      [&](const ArrayT<StringT>&) { result = Datatype::Array; },
      [&](const ArrayT<TimeT>&) { result = Datatype::Array; },
      [&](const ArrayT<TimeSpanT>&) { result = Datatype::Array; },
      // record types
      [&](const RecordT<BooleanT>&) { result = Datatype::Record; },
      [&](const RecordT<UIntegerT>&) { result = Datatype::Record; },
      [&](const RecordT<IntegerT>&) { result = Datatype::Record; },
      [&](const RecordT<FloatT>&) { result = Datatype::Record; },
      [&](const RecordT<OctetStringT>&) { result = Datatype::Record; },
      [&](const RecordT<StringT>&) { result = Datatype::Record; },
      [&](const RecordT<TimeT>&) { result = Datatype::Record; },
      [&](const RecordT<TimeSpanT>&) { result = Datatype::Record; });
  return result;
}

struct Unit : public NamedAttribute {
  Unit() = default;

  Unit(uint16_t code, const std::string& abbr) : code_(code), abbr_(abbr) {}

  Unit(uint16_t code, const std::string& abbr, std::optional<TextID>&& name)
      : NamedAttribute(std::move(name)), code_(code), abbr_(abbr) {}

  size_t hash() const noexcept { return std::hash<uint16_t>{}(code_); }

  uint16_t code() const { return code_; }

  std::string abbr() const { return abbr_; }

private:
  uint16_t code_;
  std::string abbr_;
};

using SimpleDatatypeValue = // TimeT and TimeSpanT are stored as strings
    std::variant<bool, uint64_t, int64_t, float, std::string>;

inline void expand(DataValue& lhs, const DataValue& rhs) {
  if (lhs.index() != rhs.index()) {
    throw std::logic_error(
        "Expanded data type does not match variable datatype");
  }
  match(
      lhs,
      [&rhs](BooleanT value) { value.expand(std::get<BooleanT>(rhs)); },
      [&rhs](UIntegerT value) { value.expand(std::get<UIntegerT>(rhs)); },
      [&rhs](IntegerT value) { value.expand(std::get<IntegerT>(rhs)); },
      [&rhs](FloatT value) { value.expand(std::get<FloatT>(rhs)); },
      [&rhs](OctetStringT value) { /* no expansion for OctetStringT */ },
      [&rhs](StringT value) { /* no expansion for StringT */ },
      [&rhs](TimeT value) { /* no expansion for TimeT */ },
      [&rhs](TimeSpanT value) { /* no expansion for TimeSpanT */ },
      // array types
      [&rhs](ArrayT<BooleanT> value) {
        value.expand(std::get<ArrayT<BooleanT>>(rhs));
      },
      [&rhs](ArrayT<UIntegerT> value) {
        value.expand(std::get<ArrayT<UIntegerT>>(rhs));
      },
      [&rhs](ArrayT<IntegerT> value) {
        value.expand(std::get<ArrayT<IntegerT>>(rhs));
      },
      [&rhs](ArrayT<FloatT> value) {
        value.expand(std::get<ArrayT<FloatT>>(rhs));
      },
      [&rhs](ArrayT<OctetStringT> value) {
        value.expand(std::get<ArrayT<OctetStringT>>(rhs));
      },
      [&rhs](ArrayT<StringT> value) {
        value.expand(std::get<ArrayT<StringT>>(rhs));
      },
      [&rhs](
          ArrayT<TimeT> value) { value.expand(std::get<ArrayT<TimeT>>(rhs)); },
      [&rhs](ArrayT<TimeSpanT> value) {
        value.expand(std::get<ArrayT<TimeSpanT>>(rhs));
      },
      // record types
      [&rhs](RecordT<BooleanT> value) {
        value.expand(std::get<RecordT<BooleanT>>(rhs));
      },
      [&rhs](RecordT<UIntegerT> value) {
        value.expand(std::get<RecordT<UIntegerT>>(rhs));
      },
      [&rhs](RecordT<IntegerT> value) {
        value.expand(std::get<RecordT<IntegerT>>(rhs));
      },
      [&rhs](RecordT<FloatT> value) {
        value.expand(std::get<RecordT<FloatT>>(rhs));
      },
      [&rhs](RecordT<OctetStringT> value) {
        value.expand(std::get<RecordT<OctetStringT>>(rhs));
      },
      [&rhs](RecordT<StringT> value) {
        value.expand(std::get<RecordT<StringT>>(rhs));
      },
      [&rhs](RecordT<TimeT> value) {
        value.expand(std::get<RecordT<TimeT>>(rhs));
      },
      [&rhs](RecordT<TimeSpanT> value) {
        value.expand(std::get<RecordT<TimeSpanT>>(rhs));
      });
}

struct Variable {
  Variable() = default;

  Variable(size_t index,
      TextID&& name,
      AccessRights access,
      DataValue&& value,
      std::optional<TextID>&& desc = std::nullopt,
      std::optional<SimpleDatatypeValue> default_value = std::nullopt,
      bool dynamic = false,
      bool modifies_others = false,
      bool excluded = false)
      : index_(index), name_(std::move(name)), access_(access),
        value_(std::move(value)), desc_(std::move(desc)),
        default_(std::move(default_value)), dynamic_(dynamic),
        modifies_others_(modifies_others), excluded_(excluded) {}

  Variable(const Variable& other,
      std::optional<SimpleDatatypeValue> default_value,
      std::optional<bool> excluded,
      std::optional<DataValue> value)
      : index_(other.index_), name_(other.name_), access_(other.access_),
        value_(other.value_), desc_(other.desc_),
        default_((default_value ? default_value.value() : other.default_)),
        dynamic_(other.dynamic_), modifies_others_(other.modifies_others_),
        excluded_((excluded ? excluded.value() : other.excluded_)) {
    if (value.has_value()) {
      expand(value_, *value);
    }
  }

  size_t index() const { return index_; }

  TextID name() const { return name_; }

  AccessRights access() const { return access_; }

  DataValue value() const { return value_; }

  Datatype type() const { return toDatatype(value_); }

  SimpleDatatypeValue defaultValue() const {
    if (default_.has_value()) {
      return default_.value();
    } else {
      throw std::runtime_error(name_.id() + " Variable has no default value");
    }
  }

  std::optional<TextID> description() const { return desc_; }

  bool dynamic() const { return dynamic_; }

  bool modifiesOthers() const { return modifies_others_; }

  bool excluded() const { return excluded_; }

private:
  size_t index_;
  TextID name_;
  AccessRights access_;
  DataValue value_;
  std::optional<TextID> desc_;
  std::optional<SimpleDatatypeValue> default_;
  bool dynamic_;
  bool modifies_others_;
  bool excluded_;
};

using VariablePtr = std::shared_ptr<Variable>;

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
  return lhs.value() == rhs.value();
}

template <typename T>
inline bool operator!=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() != rhs.value();
}

template <typename T>
inline bool operator<=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() <= rhs.value();
}

template <typename T>
inline bool operator>=(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() >= rhs.value();
}

template <typename T>
inline bool operator<(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() < rhs.value();
}

template <typename T>
inline bool operator>(const SingleValue<T>& lhs, const SingleValue<T>& rhs) {
  return lhs.value() > rhs.value();
}

template <typename T>
inline bool operator==(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() == rhs.lower()) && (lhs.upper() == rhs.upper());
}

template <typename T>
inline bool operator!=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() != rhs.lower()) && (lhs.upper() != rhs.upper());
}

template <typename T>
inline bool operator<=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() <= rhs.lower()) && (lhs.upper() <= rhs.upper());
}

template <typename T>
inline bool operator>=(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() >= rhs.lower()) && (lhs.upper() >= rhs.upper());
}

template <typename T>
inline bool operator<(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() < rhs.lower()) && (lhs.upper() < rhs.upper());
}

template <typename T>
inline bool operator>(const ValueRange<T>& lhs, const ValueRange<T> rhs) {
  return (lhs.lower() > rhs.lower()) && (lhs.upper() > rhs.upper());
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

inline bool operator==(const Unit& lhs, const Unit& rhs) {
  return lhs.code() == rhs.code();
}

inline bool operator!=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() != rhs.code();
}

inline bool operator<=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() <= rhs.code();
}

inline bool operator>=(const Unit& lhs, const Unit& rhs) {
  return lhs.code() >= rhs.code();
}

inline bool operator<(const Unit& lhs, const Unit& rhs) {
  return lhs.code() < rhs.code();
}

inline bool operator>(const Unit& lhs, const Unit& rhs) {
  return lhs.code() > rhs.code();
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