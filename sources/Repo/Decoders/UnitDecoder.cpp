#include "UnitDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "XML_Helper.hpp"

using namespace std;
using namespace pugi;

namespace IODD {
UnitsMapPtr decodeUnits(const filesystem::path& path) {
  auto result = make_shared<UnitsMap>();

  auto doc = getXML(path);
  auto xml = getXMLNode("IODDStandardUnitDefinitions", doc, path);
  auto locales = getXMLNode(
      vector<string>{"ExternalTextCollection", "PrimaryLanguage"}, xml, path);
  auto units_collection = getXMLNode("UnitCollection", xml, path);

  for (const auto& unit_node : units_collection.children("Unit")) {
    try {
      auto code = getXMLAttribute("code", unit_node).as_uint();
      string abbr = getXMLAttribute("abbr", unit_node).as_string();
      string id = getXMLAttribute("textId", unit_node).as_string();
      auto locale = decodeLocalization(locales, id);
      auto unit = make_shared<Unit>(locale, code, abbr);
      result->emplace(code, move(unit));
    } catch (const AttributeNotFound& ex) {
      throw runtime_error("Failed to decode unit " +
          to_string(unit_node.offset_debug()) + " from " + path.string() +
          " due to exception: " + string(ex.what()));
    }
  }
  return result;
}

UnitPtr findUnit(uint16_t id, const UnitsMapPtr& units) {
  if (auto it = units->find(id); it != units->end()) {
    return it->second;
  }
  return nullptr;
}

UnitPtr decodeUnitPtr(const UnitsMapPtr& units, const xml_node& xml) {
  if (auto unit_attribute = xml.attribute("unitCode");
      !unit_attribute.empty()) {
    auto unit_id = unit_attribute.as_uint();
    return findUnit(unit_id, units);
  }
  return nullptr;
}

} // namespace IODD