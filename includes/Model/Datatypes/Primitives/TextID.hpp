#ifndef __IODD_STANDARD_DEFINES_TEXT_ID_HPP
#define __IODD_STANDARD_DEFINES_TEXT_ID_HPP

#include <string>

namespace IODD {

struct TextID {
  TextID(const std::string& id, const std::string& locale);

  ~TextID() = default;

  std::string id() const;

  std::string locale() const;

  explicit operator bool() const;

private:
  std::string id_;
  std::string locale_;
};
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_TEXT_ID_HPP
