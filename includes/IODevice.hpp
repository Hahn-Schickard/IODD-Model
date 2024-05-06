#ifndef __IODD_IO_DEVICE_HPP
#define __IODD_IO_DEVICE_HPP

#include <optional>
#include <string>
#include <unordered_map>

namespace IODD {

struct DeviceIdentity {
  const std::string vendor_id;
  const std::string vendor_name;
  const std::string device_id;
  // vendor_text
  // vendor_url
  // vendor_logo
  // device_family
  // DeviceVariantCollection{}
};

enum class Datatype {
  Boolean,
  UInteger, // bit length from 2 to 64
  Integer,
  Float32,
  String, // length + encoding, special char encoding for & ‘ < > "
  OctetString, // length
  Time, // yyyy-mm-dd[Thh:mm:ss[.fff]]
  TimeSpan, //±PnYn MnDTnH nMnS - std::duration?
  Array, // size,
  Record, // bit length from 1 to 1856
  ProcessDataIn,
  ProcessDataOut
};

struct DeviceFunction {
  // probably useless
  struct Features {
    const bool block_param;
    const bool data_storage;
    const std::string profile_char;
    struct SupportedAccessLocks {
      const bool local_ui;
      const bool data_storage;
      const bool param;
      const bool local_param;
    };
  };
  struct VariableCollection {};
};

struct IODevice {
  const DeviceIdentity id;
};
} // namespace IODD

#endif // __IODD_IO_DEVICE_HPP