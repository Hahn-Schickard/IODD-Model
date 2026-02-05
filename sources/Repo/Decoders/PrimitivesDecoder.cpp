#include "PrimitivesDecoder.hpp"
#include "XML_Helper.hpp"

using namespace std;
using namespace pugi;

namespace IODD {
TextID decodeLocalization(const xml_node& locales, const string& text_id) {
  string localization =
      locales.find_child_by_attribute("Text", "id", text_id.c_str())
          .attribute("value")
          .as_string();
  return TextID(text_id, localization);
}

optional<AccessRights> decodeAccessRights(const xml_node& node) {
  try {
    string access_string = node.attribute("accessRights").as_string();
    if (access_string.empty()) {
      access_string =
          getXMLAttribute("accessRightRestriction", node).as_string();
    }
    if (access_string == "ro") {
      return AccessRights::ReadOnly;
    } else if (access_string == "wo") {
      return AccessRights::WriteOnly;
    } else if (access_string == "rw") {
      return AccessRights::ReadWrite;
    } else {
      return nullopt; // @todo: handle malformed access string
    }
  } catch (const AttributeNotFound&) {
    return nullopt;
  }
}

optional<TextID> decodeLocalizedText(
    const string& child_name, const xml_node& node, const xml_node& locales) {
  auto name_node = node.child(child_name.c_str());
  if (!name_node.empty()) {
    try {
      return decodeLocalization(
          locales, getXMLAttribute("textId", name_node).as_string());
    } catch (const exception&) {
      throw runtime_error(
          "Could not get localization for " + child_name + " field");
    }
  }
  return nullopt;
}
} // namespace IODD