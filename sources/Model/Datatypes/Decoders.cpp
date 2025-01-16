#include "Decoders.hpp"

#include <HSCUL/FloatingPoint.hpp>
#include <HSCUL/Integer.hpp>
#include <HSCUL/String.hpp>
#include <Variant_Visitor.hpp>
#include <date/date.h>

#include <algorithm>
#include <stdexcept>

using namespace std;

namespace IODD {

// NOLINTBEGIN(readability-magic-numbers)
bool decodeValue(const vector<uint8_t>& bytes, const BooleanT_Ptr&) {
  if (bytes.size() != 1) {
    throw logic_error("");
  }
  return bytes[0] > 0;
}

uint64_t decodeValue(const vector<uint8_t>& bytes, const UIntegerT_Ptr&) {
  if (bytes.empty()) {
    throw logic_error("");
  }
  if (bytes.size() > 8) {
    throw logic_error("");
  }
  return HSCUL::toUnsignedInteger(bytes);
}

int64_t decodeValue(const vector<uint8_t>& bytes, const IntegerT_Ptr&) {
  if (bytes.empty()) {
    throw logic_error("");
  }
  if (bytes.size() > 8) {
    throw logic_error("");
  }
  return HSCUL::toSignedInteger(bytes);
}

float decodeValue(const vector<uint8_t>& bytes, const FloatT_Ptr&) {
  if (bytes.empty()) {
    throw logic_error("");
  }
  if (bytes.size() != 4) {
    throw logic_error("");
  }
  return HSCUL::toFloat(bytes);
}

string decodeValue(const vector<uint8_t>& bytes, const OctetStringT_Ptr&) {
  return HSCUL::hexify(bytes, false, " ");
}

string decodeValue(const vector<uint8_t>& bytes, const StringT_Ptr& type) {
  if (bytes.size() != type->length()) {
    throw logic_error("");
  }
  return HSCUL::toString(bytes);
}

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

string decodeValue(const vector<uint8_t>& bytes, const TimeT_Ptr&) {
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr uint32_t next_epoch_marker = 0x9DFF4400;
  if (bytes.size() != 8) {
    throw logic_error("Input must be 8 bytes long for correct TimeT decoding");
  }
  auto it = bytes.rbegin();
  advance(it, 3);
  auto seconds = HSCUL::toUnsignedInteger(vector<uint8_t>(bytes.rbegin(), it));
  auto timepoint_s = chrono::time_point<chrono::system_clock, chrono::seconds>(
      chrono::seconds(seconds));
  shiftEpoch(timepoint_s, (seconds < next_epoch_marker));

  advance(it, 1);
  auto subseconds = HSCUL::toUnsignedInteger(vector<uint8_t>(it, bytes.rend()));

  auto timepoint = timepoint_s + Fractional(subseconds);
  return date::format("%FT%TZ", timepoint); // ISO 8601 DateTime
}

string decodeValue(const vector<uint8_t>& bytes, const TimeSpanT_Ptr&) {
  if (bytes.size() != 8) {
    throw logic_error(
        "Input must be 8 bytes long for correct TimeSpanT decoding");
  }
  auto tmp = HSCUL::toSignedInteger(bytes);
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

SimpleDatatypeValue decodeValue(
    const vector<uint8_t>& bytes, const SimpleDatatype& type) {
  SimpleDatatypeValue result;
  // clang-format off
  // match(type, 
  //   [&result, bytes](const BooleanT_Ptr& value) {
  //     result = decodeValue(bytes, value);
  //   }
  // );
  // clang-format on
  match(type, [&result, bytes](const auto& value) {
    result = decodeValue(bytes, value);
  });

  return result;
}

std::vector<bool> toBitVector(const std::vector<uint8_t>& bytes) {
  constexpr uint8_t BYTE_SIZE = 8;
  std::vector<bool> result;
  result.reserve(bytes.size() * BYTE_SIZE);
  for (auto byte : bytes) {
    for (uint8_t bit = 0; bit < BYTE_SIZE; ++bit) {
      result.emplace_back((byte >> bit) & 1);
    }
  }
  return result;
}

std::vector<uint8_t> toByteVector(const std::vector<bool>& bits) {
  constexpr uint8_t BYTE_SIZE = 8;
  // NOLINTNEXTLINE(readability-magic-numbers)
  std::vector<uint8_t> result((bits.size() + 7) / BYTE_SIZE);
  for (size_t i = 0; i < bits.size(); ++i) {
    uint8_t byte_index = i / BYTE_SIZE;
    uint8_t bit_index = i % BYTE_SIZE;
    result[byte_index] |= (bits[i] << bit_index);
  }
  return result;
}

std::vector<uint8_t> bitwiseView(
    const std::vector<uint8_t>& bytes, uint8_t subindex, size_t bit_length) {
  auto bits = toBitVector(bytes);
  std::reverse(bits.begin(), bits.end());

  size_t offset = subindex * bit_length;
  auto begining = bits.begin() + offset;
  auto end = begining + bit_length;
  std::vector<bool> subbits(begining, end);

  return toByteVector(subbits);
}

std::vector<uint8_t> bytewiseView(
    const std::vector<uint8_t>& bytes, uint8_t subindex, size_t byte_length) {
  auto offset = subindex * byte_length;
  auto tmp = bytes;
  std::reverse(tmp.begin(), tmp.end());
  auto begining = tmp.begin() + offset;
  auto end = begining + byte_length;
  return std::vector<uint8_t>(begining, end);
}

SimpleDatatypeValue decodeValue(const std::vector<uint8_t>& bytes,
    const ArrayT_Ptr& type,
    uint8_t subindex) {
  if (subindex == 0) {
    throw runtime_error("Decoding ArrayT as a whole is not supported");
  } else {
    if (!type->subindexAccess()) {
      throw runtime_error("Given ArrayT does not support subindex access");
    }

    --subindex; // decrement to use standard index notation
    // clang-format off
    auto result = match(type->type(), 
      [bytes, subindex](const BooleanT_Ptr&) -> SimpleDatatypeValue {
        auto bits = toBitVector(bytes);
        std::reverse(bits.begin(), bits.end());
        return bits[subindex];
      },
      [bytes, subindex](const UIntegerT_Ptr& type) -> SimpleDatatypeValue {
        return decodeValue(
          bitwiseView(bytes, subindex, type->bitLength()), type);
      },
      [bytes, subindex](const IntegerT_Ptr& type) -> SimpleDatatypeValue {
        return decodeValue(
          bitwiseView(bytes, subindex, type->bitLength()), type);
      },
      [bytes, subindex](const auto& type) -> SimpleDatatypeValue {
        return decodeValue(bytewiseView(bytes, subindex, type->length()), type);
      }
    ); // clang-format on
    return result;
  }
}

SimpleDatatypeValue decodeValue(const std::vector<uint8_t>& bytes,
    const RecordT_Ptr& type,
    uint8_t subindex) {}
} // namespace IODD
