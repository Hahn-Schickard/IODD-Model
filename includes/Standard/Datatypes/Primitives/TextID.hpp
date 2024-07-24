#ifndef __IODD_STANDARD_DEFINES_TEXT_ID_HPP
#define __IODD_STANDARD_DEFINES_TEXT_ID_HPP

#include <stdexcept>
#include <string>

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