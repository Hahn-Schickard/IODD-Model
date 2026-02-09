#ifndef __IODD_STANDARD_DEFINES_PROCESS_DATA_T_HPP
#define __IODD_STANDARD_DEFINES_PROCESS_DATA_T_HPP

#include "Datatypes.hpp"
#include "Primitives/FixedBitLength.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace IODD {

struct ProcessDataT : FixedBitLength<1, 256> {
  ProcessDataT(const std::string& id,
      uint16_t bits,
      const TextIDPtr& name,
      DataValue&& value);

  ~ProcessDataT() = default;

  std::string id() const;

  TextIDPtr name() const;

  DataValue value() const;

  NamedAttributePtr valueName(const SimpleDatatypeValue& value,
      std::optional<uint8_t> subindex = std::nullopt) const;

  Datatype type() const;

private:
  std::string id_;
  TextIDPtr name_;
  DataValue value_;
};

using ProcessDataTPtr = std::shared_ptr<ProcessDataT>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_PROCESS_DATA_T_HPP