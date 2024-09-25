#include "ConditionDecoder.hpp"
#include "XML_Helper.hpp"

#include <cstdint>

using namespace std;
using namespace pugi;

namespace IODD {
optional<Condition> decodeCondition(
    const xml_node& xml, const VariablesMap& variables) {
  auto condition_xml = xml.child("Condition");
  if (!condition_xml.empty()) {
    string var_id = getXMLAttribute("variableId", condition_xml).as_string();
    auto var = findVariable(var_id, variables);

    std::optional<uint8_t> condition_subindex = nullopt;
    auto subindex_attribute = condition_xml.attribute("subindex");
    if (subindex_attribute.empty()) {
      condition_subindex = subindex_attribute.as_uint();
    }

    uint8_t condition_value = getXMLAttribute("value", condition_xml).as_uint();

    return Condition(var, condition_subindex, condition_value);
  } else {
    return nullopt;
  }
}
} // namespace IODD
