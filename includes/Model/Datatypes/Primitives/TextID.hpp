#ifndef __IODD_STANDARD_DEFINES_TEXT_ID_HPP
#define __IODD_STANDARD_DEFINES_TEXT_ID_HPP

#include <string>

namespace IODD {

struct TextID {
  TextID(const std::string& id, const std::string& locale);

  std::string id() const;

  std::string locale() const;

  size_t hash() const noexcept;

  explicit operator bool() const;

private:
  std::string id_;
  std::string locale_;
};

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
} // namespace IODD

template <> struct std::hash<IODD::TextID> {
  std::size_t operator()(const IODD::TextID& object) const noexcept {
    return object.hash();
  }
};

#endif //__IODD_STANDARD_DEFINES_TEXT_ID_HPP