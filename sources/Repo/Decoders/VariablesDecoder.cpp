#include "VariablesDecoder.hpp"
#include "DataValueDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "XML_Helper.hpp"

using namespace std;
using namespace pugi;

namespace IODD {
VariablesMap decodeVariables(const xml_node& xml,
    const xml_node& locales,
    const DatatypesMap& datatypes,
    const VariablesMap& std_variables) {
  VariablesMap variables = std_variables;
  for (auto variable : xml.children("Variable")) {
    string id = getXMLAttribute("id", variable).as_string();
    auto index = getXMLAttribute("index", variable).as_ullong();

    auto name_locale = decodeLocalizedText("Name", variable, locales);
    if (!name_locale.has_value()) {
      throw runtime_error("Variable " + id + " does no have name localization");
    }

    auto access = decodeAccessRights(variable);
    if (!access.has_value()) {
      throw runtime_error(
          "Variable " + id + " does no have access rights specification");
    }

    try {
      auto description = decodeLocalizedText("Description", variable, locales);
      auto dynamic = variable.attribute("dynamic").as_bool(false);
      auto modifies_others =
          variable.attribute("modifiesOtherVariables").as_bool(false);
      auto historized =
          variable.attribute("excludedFromDataStorage").as_bool(false);
      DataValue data_value;
      try {
        auto datatype_xml = getXMLNode("Datatype", variable);
        data_value = decodeDataValue(datatype_xml, locales, datatypes);
      } catch (const NodeNotFound& ex) {
        string datatype_ref_id =
            getXMLAttribute("datatypeId", getXMLNode("DatatypeRef", variable))
                .as_string();
        auto it = datatypes.find(datatype_ref_id);
        if (it != datatypes.end()) {
          data_value = it->second;
        } else {
          // NOLINTNEXTLINE(performance-inefficient-string-concatenation)
          throw runtime_error("Variable " + id + " requires DataTypeRef " +
              datatype_ref_id + ", but it does not exist");
        }
      }
      variables.emplace(id,
          make_shared<Variable>(index,
              move(name_locale.value()),
              access.value(),
              move(data_value),
              move(description),
              nullopt,
              dynamic,
              modifies_others,
              historized));
    } catch (const exception& ex) {
      throw runtime_error("Could not process variable " + id +
          " due to exception: " + ex.what());
    }
  }
  return variables;
}

SimpleDatatypeValue decodeDefaultValue(
    Datatype type, const xml_attribute& attribute) {
  switch (type) {
  case Datatype::Boolean: {
    return attribute.as_bool();
  }
  case Datatype::UInteger: {
    return (uint64_t)attribute.as_ullong();
  }
  case Datatype::Integer: {
    return (int64_t)attribute.as_llong();
  }
  case Datatype::Float32: {
    return attribute.as_float();
  }
    // NOLINTNEXTLINE(bugprone-branch-clone)
  case Datatype::String: {
    [[fallthrough]];
  }
  case Datatype::Time: {
    [[fallthrough]];
  }
  case Datatype::TimeSpan: {
    [[fallthrough]];
  }
  case Datatype::OctetString: {
    return attribute.as_string();
  }
  default: {
    throw invalid_argument(toString(type) + " is not a simple data type");
  }
  }
}

optional<DataValue> getUpdatedValues(IODD::Datatype type,
    const DatatypesMapPtr& /* datatypes */,
    const xml_node& node,
    const xml_node& locales) {
  if (!node.children().empty()) {
    if (type == IODD::Datatype::Boolean || isNumericData(type)) {
      auto simple_value = decodeSimpleDataValue(type, node, locales);
      return variantCast(simple_value);
    } else {
      // @TODO:handle Complex data types like StdRecordItemRef here
    }
  }
  return nullopt;
}

VariablesMap decodeStdVariables(const xml_node& xml,
    const xml_node& locales,
    const DatatypesMapPtr& datatypes,
    const VariablesMapPtr& std_variables) {
  VariablesMap variables = *std_variables;
  for (auto variable : xml.children("StdVariableRef")) {
    string id = variable.attribute("id").as_string();
    // ignore parameter overlays
    try {
      if (id != "V_DirectParameters_1" && id != "V_DirectParameters_2") {
        if (auto std_variable = variables.find(id);
            std_variable != variables.end()) {
          optional<SimpleDatatypeValue> default_value = nullopt;
          if (auto node_value = variable.attribute("defaultValue");
              !node_value.empty() &&
              // @TODO: implement support for nonSimpleData default values
              isSimpleData(std_variable->second->type())) {
            default_value =
                decodeDefaultValue(std_variable->second->type(), node_value);
          }
          optional<bool> excluded = nullopt;
          if (auto historized = variable.attribute("excludedFromDataStorage");
              !historized.empty()) {
            excluded = historized.as_bool();
          }
          auto possible_value = getUpdatedValues(
              std_variable->second->type(), datatypes, variable, locales);
          // check if variable needs to be updated
          if (default_value.has_value() || excluded.has_value() ||
              possible_value.has_value()) {
            std_variable->second = make_shared<Variable>(
                *std_variable->second, default_value, excluded, possible_value);
          }
        } else {
          throw runtime_error(id + " is not defined in standard variables");
        }
      }
    } catch (const exception& ex) {
      throw runtime_error("Failed to decode StdVariableRef " + id +
          " due to exception: " + ex.what());
    }
  }
  return variables;
}
} // namespace IODD
