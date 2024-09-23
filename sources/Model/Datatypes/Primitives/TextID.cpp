#include "TextID.hpp"

#include <stdexcept>

using namespace std;

namespace IODD {

TextID::TextID(const string& id, const string& locale)
    : id_(id), locale_(locale) {
  if (id_.empty()) {
    throw invalid_argument(
        "Failed to create TextID. ID argument can not be empty");
  }
  if (locale_.empty()) {
    throw invalid_argument("Failed to create TextID " + id_ +
        ". Locale argument can not be empty");
  }
}

string TextID::id() const { return id_; }

string TextID::locale() const { return locale_; }

size_t TextID::hash() const noexcept { return std::hash<string>{}(id_); }

TextID::operator bool() const { return !id_.empty() && !locale_.empty(); }

} // namespace IODD
