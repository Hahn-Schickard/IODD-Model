#include "Menu.hpp"

using namespace std;

namespace IODD {
Menu::Menu(const string& id,
    vector<Ref>&& references,
    const TextIDPtr& name,
    const optional<Condition>& condition)
    : id_(id), references_(move(references)), name_(name),
      condition_(condition) {
  if (id_.empty()) {
    throw invalid_argument("Menu ID can not be empty");
  }
  if (references_.empty()) {
    throw invalid_argument("Menu References can not be empty");
  }
}

size_t Menu::size() const { return references_.size(); }

string Menu::id() const { return id_; }

Menu::Refs Menu::references() const { return references_; }

TextIDPtr Menu::name() const { return name_; }

TextIDPtr Menu::tryName() const {
  if (!name_) {
    throw MenuHasNoName(id_);
  }
  return name_;
}

optional<Condition> Menu::condition() const { return condition_; }

} // namespace IODD
