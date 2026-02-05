#include "Decoders.hpp"

#include <Variant_Visitor/Visitor.hpp>
#include <date/date.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace IODD {

static constexpr uint8_t BYTE_SIZE = 8;

bool isLittleEndian() {
  unsigned int i = 1;
  // if dereferenced pointer starts with a 1, system is little endian, because
  // little endian systems are LSB first, and in this simple case the LSB is 1
  return (*((char*)&i) != 0);
}

intmax_t toSignedInteger(vector<uint8_t> bytes) {
  if (bytes.size() > sizeof(intmax_t)) {
    string error_msg = "Given byte array does not fit into largest "
                       "available signed integer. "
                       "Largest signed integer is " +
        to_string(sizeof(intmax_t)) + " bytes long, but your given array is " +
        to_string(bytes.size()) + " bytes long";
    throw overflow_error(error_msg);
  }

  const uint8_t msb_mask = 0x80;
  const uint8_t negative_marker = 0xFF;
  auto padding_size = sizeof(intmax_t) - bytes.size();

  if (!isLittleEndian() && padding_size != 0) {
    // msb
    if ((bytes.front() & msb_mask) == msb_mask) {
      // negative integer
      bytes.insert(bytes.begin(), padding_size, negative_marker);
    }
  } else {
    // lsb
    if ((bytes.back() & msb_mask) == msb_mask) {
      // negative integer
      bytes.insert(bytes.end(), padding_size, negative_marker);
    }
  }

  intmax_t result = 0;
  copy(bytes.begin(), bytes.end(), reinterpret_cast<uint8_t*>(&result));
  return result;
}

size_t toUnsignedInteger(vector<uint8_t> bytes) {
  if (bytes.size() > sizeof(size_t)) {
    string error_msg = "Given byte array does not fit into largest "
                       "available unsigned integer. "
                       "Largest unsigned integer is " +
        to_string(sizeof(size_t)) + " bytes long, but your given array is " +
        to_string(bytes.size()) + " bytes long";
    throw overflow_error(error_msg);
  }
  size_t result = 0;
  copy(bytes.begin(), bytes.end(), reinterpret_cast<uint8_t*>(&result));
  return result;
}

float toFloat(vector<uint8_t> bytes) {
  if (bytes.size() != sizeof(float)) {
    string error_msg =
        "Given byte array is not the size of a float type. Float is " +
        to_string(sizeof(float)) +
        " bytes long, while given byte array is: " + to_string(bytes.size()) +
        " bytes long";
    throw length_error(error_msg);
  }
  float result;
  copy(bytes.begin(), bytes.end(), reinterpret_cast<uint8_t*>(&result));
  return result;
}

string hexify(vector<uint8_t> bytes,
    bool single_prefix = true,
    const string& separator = "") {
  stringstream ss;
  ss << hex << setfill('0');
  if (single_prefix) {
    ss << "0x";
  }
  for (auto it = bytes.begin(); it != bytes.end(); ++it) {
    if (!single_prefix) {
      ss << "0x" << setw(2);
    }
    ss << static_cast<unsigned>(*it);
    if (next(it) != bytes.end()) {
      ss << separator;
    }
  }
  return ss.str();
}

string toString(vector<uint8_t> bytes) {
  return string(bytes.begin(), bytes.end());
}

bool decode(const vector<uint8_t>& bytes, const BooleanT_Ptr&) {
  if (bytes.size() != 1) {
    throw invalid_argument(
        "Input must be 8 bytes long for correct BooleanT decoding");
  }
  return bytes[0] > 0;
}

uint64_t decode(const vector<uint8_t>& bytes, const UIntegerT_Ptr&) {
  if (bytes.empty()) {
    throw invalid_argument(
        "Input can not be empty for correct UIntegerT_Ptr decoding");
  }
  if (bytes.size() > BYTE_SIZE) {
    throw invalid_argument(
        "Input can not exceed 8 bytes for correct UIntegerT_Ptr decoding");
  }
  return toUnsignedInteger(bytes);
}

int64_t decode(const vector<uint8_t>& bytes, const IntegerT_Ptr&) {
  if (bytes.empty()) {
    throw invalid_argument(
        "Input can not be empty for correct IntegerT_Ptr decoding");
  }
  if (bytes.size() > BYTE_SIZE) {
    throw invalid_argument(
        "Input can not exceed 8 bytes for correct IntegerT_Ptr decoding");
  }
  return toSignedInteger(bytes);
}

float decode(const vector<uint8_t>& bytes, const FloatT_Ptr&) {
  if (bytes.size() != 4) {
    throw invalid_argument(
        "Input must be 8 bytes long for correct FloatT_Ptr decoding");
  }
  return toFloat(bytes);
}

string decode(const vector<uint8_t>& bytes, const OctetStringT_Ptr& type) {
  if (bytes.size() != type->length()) {
    throw invalid_argument("Input must match the specified type length for "
                           "correct OctetStringT_Ptr decoding");
  }
  return hexify(bytes, false, " ");
}

string decode(const vector<uint8_t>& bytes, const StringT_Ptr& type) {
  if (bytes.size() > type->length()) {
    throw invalid_argument("Input can not exceed the specified type length "
                           "for correct StringT_Ptr decoding");
  }
  if (bytes.empty()) {
    return string();
  }
  return toString(bytes);
}

// NOLINTBEGIN(readability-magic-numbers)
void shiftEpoch(chrono::time_point<chrono::system_clock, chrono::seconds>& t,
    bool next_epoch = false) {
  using namespace date;
  auto epoch_start =
      (next_epoch ? (sys_days(February / 7 / 2036) + 6h + 28min + 16s)
                  : sys_days(January / 1 / 1984));
  auto epoch_shift = epoch_start - sys_days(January / 1 / 1970);
  t + epoch_shift;
}

// NOLINTNEXTLINE(readability-identifier-naming)
static constexpr size_t _2pow32 = 4294967296;
using Fractional = chrono::duration<int64_t, ratio<1, _2pow32>>;

string decode(const vector<uint8_t>& bytes, const TimeT_Ptr&) {
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr uint32_t next_epoch_marker = 0x9DFF4400;
  if (bytes.size() != BYTE_SIZE) {
    throw invalid_argument(
        "Input must be 8 bytes long for correct TimeT decoding");
  }
  auto it = bytes.rbegin();
  advance(it, 3);
  auto seconds = toUnsignedInteger(vector<uint8_t>(bytes.rbegin(), it));
  auto timepoint_s = chrono::time_point<chrono::system_clock, chrono::seconds>(
      chrono::seconds(seconds));
  shiftEpoch(timepoint_s, (seconds < next_epoch_marker));

  advance(it, 1);
  auto subseconds = toUnsignedInteger(vector<uint8_t>(it, bytes.rend()));

  auto timepoint = timepoint_s + Fractional(subseconds);
  return date::format("%FT%TZ", timepoint); // ISO 8601 DateTime
}

string decode(const vector<uint8_t>& bytes, const TimeSpanT_Ptr&) {
  if (bytes.size() != BYTE_SIZE) {
    throw invalid_argument(
        "Input must be 8 bytes long for correct TimeSpanT decoding");
  }
  auto tmp = toSignedInteger(bytes);
  auto time_span = Fractional(tmp);

  string result = (time_span < Fractional(0) ? "-" : "");
  result += "P";

  auto years = chrono::duration_cast<date::years>(time_span);
  result += to_string(years.count()) + "Y";

  auto months = chrono::duration_cast<date::months>(time_span - years);
  result += to_string(months.count()) + "M";

  auto days = chrono::duration_cast<date::days>(time_span - years - months);
  result += to_string(days.count()) + "D";

  result += "T";

  auto hours =
      chrono::duration_cast<chrono::hours>(time_span - years - months - days);
  result += to_string(hours.count()) + "H";

  auto minutes = chrono::duration_cast<chrono::minutes>(
      time_span - years - months - days - hours);
  result += to_string(minutes.count()) + "M";

  auto seconds = chrono::duration_cast<chrono::seconds>(
      time_span - years - months - days - hours - minutes);
  auto subseconds = chrono::duration_cast<Fractional>(
      time_span - years - months - days - hours - minutes - seconds);
  result +=
      to_string(seconds.count()) + "." + to_string(subseconds.count()) + "S";

  return result; // ISO 8601 Duration
}
// NOLINTEND(readability-magic-numbers)

SimpleDatatypeValue decode(
    const vector<uint8_t>& bytes, const SimpleDatatype& type) {
  return Variant_Visitor::match(type, [bytes](const auto& value) {
    return SimpleDatatypeValue(decode(bytes, value));
  });
}

vector<bool> toBitVector(const vector<uint8_t>& bytes) {
  vector<bool> result;
  result.reserve(bytes.size() * BYTE_SIZE);
  for (auto byte : bytes) {
    for (uint8_t bit = 0; bit < BYTE_SIZE; ++bit) {
      result.emplace_back((byte >> bit) & 1);
    }
  }
  return result;
}

vector<uint8_t> toByteVector(const vector<bool>& bits) {
  // NOLINTNEXTLINE(readability-magic-numbers)  round up for division
  vector<uint8_t> result((bits.size() + 7) / BYTE_SIZE);
  for (size_t i = 0; i < bits.size(); ++i) {
    uint8_t byte_index = i / BYTE_SIZE;
    uint8_t bit_index = i % BYTE_SIZE;
    result[byte_index] |= (static_cast<uint8_t>(bits[i]) << bit_index);
  }
  return result;
}

vector<uint8_t> bitwiseView(
    const vector<uint8_t>& bytes, uint8_t subindex, size_t bit_length) {
  auto bits = toBitVector(bytes);
  // reverse(bits.begin(), bits.end()); // @TODO: is revere needed?

  size_t offset = subindex * bit_length;
  auto beginning = bits.begin();
  advance(beginning, offset);
  auto end = beginning;
  advance(end, bit_length);
  vector<bool> subbits(beginning, end);

  return toByteVector(subbits);
}

vector<uint8_t> bytewiseView(
    const vector<uint8_t>& bytes, uint8_t subindex, size_t byte_length) {
  auto offset = subindex * byte_length;
  auto tmp = bytes;
  // reverse(tmp.begin(), tmp.end()); // @TODO: is revere needed?
  auto beginning = tmp.begin();
  advance(beginning, offset);
  auto end = beginning;
  advance(end, byte_length);
  return vector<uint8_t>(beginning, end);
}

SimpleDatatypeValue decode(
    const vector<uint8_t>& bytes, const ArrayT_Ptr& type, uint8_t subindex) {
  --subindex; // decrement to use standard index notation
  // clang-format off
    return Variant_Visitor::match(type->type(), 
      [bytes, subindex](const BooleanT_Ptr&){
        auto bits = toBitVector(bytes);
        // reverse(bits.begin(), bits.end()); // @TODO: is revere needed?
        return SimpleDatatypeValue(static_cast<bool>(bits[subindex]));
      },
      [bytes, subindex](const UIntegerT_Ptr& value_type){
        return SimpleDatatypeValue(decode(
          bitwiseView(bytes, subindex, value_type->bitLength()), value_type)
        );
      },
      [bytes, subindex](const IntegerT_Ptr& value_type){
        return SimpleDatatypeValue(decode(
          bitwiseView(bytes, subindex, value_type->bitLength()), value_type)
        );
      },
      [bytes, subindex](const auto& value_type){
        return SimpleDatatypeValue(
          decode(bytewiseView(bytes, subindex, value_type->length()), value_type)
        );
      }
    ); // clang-format on
}

size_t getBitLength(SimpleDatatype type) {
  auto result = Variant_Visitor::match(
      type,
      [](const BooleanT_Ptr&) -> size_t { return 1; },
      [](const UIntegerT_Ptr& type) -> size_t { return type->bitLength(); },
      [](const IntegerT_Ptr& type) -> size_t { return type->bitLength(); },
      [](const auto& type) -> size_t { return type->length() * BYTE_SIZE; });
  return result;
}

SimpleDatatypeValue decode(
    const vector<uint8_t>& bytes, const RecordT_Ptr& type, uint8_t subindex) {
  auto target = type->item(subindex);

  auto bits = toBitVector(bytes);
  auto beginning = bits.begin();
  auto offset = target->offset();
  advance(beginning, offset);
  auto end = beginning;
  auto bit_length = getBitLength(target->value());
  advance(end, bit_length);
  auto subvector = toByteVector(vector<bool>(beginning, end));

  return decode(subvector, target->value());
}

// @TODO: Reuse fro Repo/Decoders/DataValueDecoder.hpp
template <class... Args> struct VariantCaster {
  std::variant<Args...> v;

  template <class... ToArgs> operator std::variant<ToArgs...>() const {
    return std::visit(
        [](auto&& arg) -> std::variant<ToArgs...> { return arg; }, v);
  }
};

template <class... Args>
auto variantCast(const std::variant<Args...>& v) -> VariantCaster<Args...> {
  return {v};
}

SimpleDatatypeValue decodeValue(
    const std::vector<uint8_t>& bytes, const SimpleDatatype& type) {
  return decodeValue(bytes, variantCast(type), std::nullopt);
}

SimpleDatatypeValue decodeValue(const vector<uint8_t>& bytes,
    const DataValue& type,
    optional<uint8_t> subindex) {
  auto rbytes = bytes;
  reverse(rbytes.begin(), rbytes.end());
  // clang-format off
  return Variant_Visitor::match(type,
      [&rbytes, &subindex](const ArrayT_Ptr& value_type){
        if (!subindex.has_value()) {
          throw invalid_argument(
            "Subindex value is required for correct ArrayT_Ptr decoding"
          );
        }
        return decode(rbytes, value_type, subindex.value());
      },
      [&rbytes, &subindex](const RecordT_Ptr& value_type){
        if (!subindex.has_value()) {
          throw invalid_argument(
            "Subindex value is required for correct RecordT_Ptr decoding"
          );
        }
        return decode(rbytes, value_type, subindex.value());
      },
      [&bytes](const auto& value_type){
        return SimpleDatatypeValue(decode(bytes, value_type));
      }); // clang-format on
}
} // namespace IODD
