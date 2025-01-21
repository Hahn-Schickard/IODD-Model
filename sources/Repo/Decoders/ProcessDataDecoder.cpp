#include "ProcessDataDecoder.hpp"
#include "ConditionDecoder.hpp"
#include "DataValueDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "XML_Helper.hpp"

using namespace std;
using namespace pugi;

namespace IODD {
ProcessDataTPtr decodeProcessDataT(const xml_node& xml,
    const xml_node& locales,
    const DatatypesMapPtr& datatypes,
    bool data_in = true) {
  string target_name = (data_in ? "ProcessDataIn" : "ProcessDataOut");
  try {
    auto node = getXMLNode(target_name, xml);
    string id = getXMLAttribute("id", node).as_string();
    uint16_t bits = getXMLAttribute("bitLength", node).as_uint();
    auto value =
        decodeDataValue(getXMLNode("Datatype", node), locales, *datatypes);
    if (auto name = decodeLocalizedText("Name", node, locales)) {
      return make_shared<ProcessDataT>(
          id, bits, move(name.value()), move(value));
    } else {
      throw runtime_error(
          "Invalid " + target_name + " " + id + ". Missing name element");
    }
  } catch (const NodeNotFound&) {
    return nullptr;
  }
}

ProcessDataCollection decodeProcessData(const xml_node& xml,
    const xml_node& locales,
    const VariablesMap& variables,
    const DatatypesMapPtr& datatypes) {
  ProcessDataCollection result;
  for (const auto& node : xml.children("ProcessData")) {
    string id = getXMLAttribute("id", node).as_string();
    auto data_in = decodeProcessDataT(node, locales, datatypes);
    auto data_out = decodeProcessDataT(node, locales, datatypes, false);
    auto condition = decodeCondition(node, variables);
    result.emplace(id,
        make_shared<ProcessDataUnion>(
            id, move(data_in), move(data_out), condition));
  }
  return result;
}
} // namespace IODD
