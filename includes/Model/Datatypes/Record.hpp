#ifndef __IODD_STANDARD_DEFINES_RECORD_T_HPP
#define __IODD_STANDARD_DEFINES_RECORD_T_HPP

#include "AccessRights.hpp"
#include "ComplexDatatype.hpp"
#include "Primitives/TextID.hpp"
#include "SimpleDataTypes.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

namespace IODD {

struct RecordItem {
  RecordItem() = default;

  RecordItem(uint8_t subindex,
      uint16_t offset,
      SimpleDatatype&& value,
      TextID&& name,
      std::optional<AccessRights> access = std::nullopt,
      std::optional<TextID>&& desc = std::nullopt);

  ~RecordItem() = default;

  uint8_t subindex() const;

  uint16_t offset() const;

  SimpleDatatype value() const;

  Datatype type() const;

  TextID name() const;

  std::optional<AccessRights> access() const;

  std::optional<TextID> description() const;

private:
  uint8_t subindex_;
  uint16_t offset_;
  SimpleDatatype value_;
  TextID name_;
  std::optional<AccessRights> access_;
  std::optional<TextID> desc_;
};

using RecordItem_Ptr = std::shared_ptr<RecordItem>;

using RecordItems = std::unordered_map<uint8_t, RecordItem_Ptr>;

struct RecordT : public FixedBitLength<1, 1856>, public ComplexDataTypeT {
  RecordT() = default;

  RecordT(uint16_t bit_length, RecordItems&& items);

  RecordT(uint16_t bit_length, bool subindex_access, RecordItems&& items);

  ~RecordT() = default;

  void expand(const RecordT& other);

  RecordItems items() const;

  RecordItem_Ptr item(uint8_t subindex) const;

private:
  RecordItems items_;
};

using RecordT_Ptr = std::shared_ptr<RecordT>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_RECORD_T_HPP
