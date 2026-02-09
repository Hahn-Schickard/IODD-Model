#ifndef __IODD_STANDARD_DEFINES_TEXT_ID_HPP
#define __IODD_STANDARD_DEFINES_TEXT_ID_HPP

#include <memory>
#include <stdexcept>
#include <string>

namespace IODD {

struct TextID {
  TextID(const std::string& id, const std::string& locale);

  ~TextID() = default;

  std::string id() const;

  std::string locale() const;

private:
  std::string id_;
  std::string locale_;
};

using TextIDPtr = std::shared_ptr<TextID>;
} // namespace IODD

#endif //__IODD_STANDARD_DEFINES_TEXT_ID_HPP
