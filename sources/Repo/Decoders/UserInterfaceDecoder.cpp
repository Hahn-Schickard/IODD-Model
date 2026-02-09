#include "UserInterfaceDecoder.hpp"
#include "ConditionDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "UnitDecoder.hpp"
#include "VariablesDecoder.hpp"
#include "XML_Helper.hpp"

#include <cstring>
#include <string>

using namespace std;
using namespace pugi;

namespace IODD {
optional<float> decodeFloatAttribute(const xml_node& xml, const string& name) {
  if (auto attribute = xml.attribute(name.c_str()); !attribute.empty()) {
    return attribute.as_float();
  }
  return nullopt;
}

unordered_map<string, string> decodeMenuIDs(const xml_node& xml) {
  unordered_map<string, string> result;
  for (const auto& child : xml.children()) {
    if (!child.attribute("menuId").empty()) {
      result.emplace(child.name(), child.attribute("menuId").as_string());
    }
  }
  return result;
}

DisplayFormat decodeDisplayFormat(const xml_node& xml) {
  if (auto format_attribute = xml.attribute("displayFormat");
      !format_attribute.empty()) {
    unordered_map<string, DisplayFormat> format_types{
        {"Dec", DisplayFormat::Dec},
        {"Dec.0", DisplayFormat::Dec0},
        {"Dec.1", DisplayFormat::Dec1},
        {"Dec.2", DisplayFormat::Dec2},
        {"Dec.3", DisplayFormat::Dec3},
        {"Dec.4", DisplayFormat::Dec4},
        {"Dec.5", DisplayFormat::Dec5},
        {"Dec.6", DisplayFormat::Dec6},
        {"Dec.7", DisplayFormat::Dec7},
        {"Dec.8", DisplayFormat::Dec8},
        {"Dec.9", DisplayFormat::Dec9},
        {"Bin", DisplayFormat::Bin},
        {"Hex", DisplayFormat::Hex}};
    return format_types.at(format_attribute.as_string());
  }
  return DisplayFormat::None;
}

VariableRef::ButtonValue decodeButtonValue(
    Datatype type, const xml_attribute& xml) {
  switch (type) {
  case Datatype::Boolean: {
    return xml.as_bool();
  }
  case Datatype::Integer: {
    return (int64_t)xml.as_llong();
  }
  case Datatype::UInteger: {
    return (uint64_t)xml.as_ullong();
  }
  default: {
    throw invalid_argument(toString(type) + " can not be used as button value");
  }
  }
}

VariableRefPtr decodeVariableRef(const UnitsMapPtr& units,
    const VariablesMap& variables,
    const xml_node& xml,
    const xml_node& locales) {
  string variable_id = getXMLAttribute("variableId", xml).as_string();
  auto variable = findVariable(variable_id, variables);

  try {
    auto button_xml = getXMLNode("Button", xml);
    auto button_value = decodeButtonValue(
        variable->type(), button_xml.attribute("buttonValue"));
    auto description = decodeLocalizedText("Description", button_xml, locales);
    auto action_msg =
        decodeLocalizedText("ActionStartedMessage", button_xml, locales);

    return make_shared<VariableRef>(
        variable, button_value, description, action_msg);
  } catch (const NodeNotFound&) {
    auto gradient = decodeFloatAttribute(xml, "gradient");
    auto offset = decodeFloatAttribute(xml, "offset");
    auto unit = decodeUnitPtr(units, xml);
    auto format = decodeDisplayFormat(xml);
    auto access = decodeAccessRights(xml);

    return make_shared<VariableRef>(
        variable, gradient, offset, unit, format, access);
  }
}

RecordRefPtr decodeRecordRef(const UnitsMapPtr& units,
    const VariablesMap& variables,
    const xml_node& xml,
    const xml_node& locales) {
  string variable_id = getXMLAttribute("variableId", xml).as_string();
  auto variable = findVariable(variable_id, variables);
  auto subindex = getXMLAttribute("subindex", xml).as_uint();

  try {
    auto button_xml = getXMLNode("Button", xml);
    auto button_value = decodeButtonValue(
        variable->type(), button_xml.attribute("buttonValue"));
    auto description = decodeLocalizedText("Description", button_xml, locales);
    auto action_msg =
        decodeLocalizedText("ActionStartedMessage", button_xml, locales);

    return make_shared<RecordRef>(
        variable, subindex, button_value, description, action_msg);
  } catch (const NodeNotFound&) {
    auto gradient = decodeFloatAttribute(xml, "gradient");
    auto offset = decodeFloatAttribute(xml, "offset");
    auto unit = decodeUnitPtr(units, xml);
    auto format = decodeDisplayFormat(xml);
    auto access = decodeAccessRights(xml);

    return make_shared<RecordRef>(
        variable, subindex, gradient, offset, unit, format, access);
  }
}

MenuPtr decodeMenu(const UnitsMapPtr& units,
    const VariablesMap& variables,
    const string& menu_id,
    const xml_node& menus,
    const xml_node& locales,
    const optional<Condition>& condition = nullopt) {
  auto xml = menus.find_child_by_attribute("Menu", "id", menu_id.c_str());

  if (strcmp(xml.name(), "Menu") != 0) {
    throw logic_error(
        "Can not decode " + menu_id + " as a Menu element. It's not a Menu");
  }

  std::vector<Menu::Ref> refs;
  optional<TextID> name = decodeLocalizedText("Name", xml, locales);

  for (const auto& child : xml.children()) {
    if (strcmp(child.name(), "VariableRef") == 0) {
      refs.emplace_back(decodeVariableRef(units, variables, child, locales));
    }
    if (strcmp(child.name(), "RecordItemRef") == 0) {
      refs.emplace_back(decodeRecordRef(units, variables, child, locales));
    }
    if (strcmp(child.name(), "MenuRef") == 0) {
      string ref_id = getXMLAttribute("menuId", child).as_string();
      auto ref_condition = decodeCondition(child, variables);

      refs.emplace_back(
          decodeMenu(units, variables, ref_id, menus, locales, ref_condition));
    }
  }
  return make_shared<Menu>(menu_id, move(refs), move(name), condition);
}

MenuPtr decodeOptionalMenu(const string& menu_name,
    unordered_map<string, string> menu_ids,
    const UnitsMapPtr& units,
    const VariablesMap& variables,
    const xml_node& xml,
    const xml_node& locales) {
  try {
    return decodeMenu(units, variables, menu_ids.at(menu_name), xml, locales);
  } catch (const out_of_range&) {
    return nullptr;
  }
}

UserInterfacePtr decodeRoleUI(UserRole role,
    unordered_map<string, string> menu_ids,
    const UnitsMapPtr& units,
    const VariablesMap& variables,
    const xml_node& xml,
    const xml_node& locales) {
  auto ident_menu = decodeMenu(
      units, variables, menu_ids.at("IdentificationMenu"), xml, locales);
  auto param_menu = decodeOptionalMenu(
      "ParameterMenu", menu_ids, units, variables, xml, locales);
  auto observe_menu = decodeOptionalMenu(
      "ObservationMenu", menu_ids, units, variables, xml, locales);
  auto diag_menu = decodeOptionalMenu(
      "DiagnosisMenu", menu_ids, units, variables, xml, locales);

  return make_shared<UserInterface>(
      role, ident_menu, param_menu, observe_menu, diag_menu);
}

unordered_map<UserRole, UserInterfacePtr> decodeUI(const UnitsMapPtr& units,
    const VariablesMap& variables,
    const xml_node& xml,
    const xml_node& locales) {
  unordered_map<UserRole, UserInterfacePtr> result;

  auto menus_xml = getXMLNode("MenuCollection", xml);

  auto observer_menu_ids =
      decodeMenuIDs(getXMLNode("ObserverRoleMenuSet", xml));
  result.emplace(UserRole::ObservationRole,
      decodeRoleUI(UserRole::ObservationRole,
          observer_menu_ids,
          units,
          variables,
          menus_xml,
          locales));

  auto maintainence_menu_ids =
      decodeMenuIDs(getXMLNode("MaintenanceRoleMenuSet", xml));
  result.emplace(UserRole::MaintenanceRole,
      decodeRoleUI(UserRole::MaintenanceRole,
          maintainence_menu_ids,
          units,
          variables,
          menus_xml,
          locales));

  auto specialist_menu_ids =
      decodeMenuIDs(getXMLNode("SpecialistRoleMenuSet", xml));
  result.emplace(UserRole::SpecialistRole,
      decodeRoleUI(UserRole::SpecialistRole,
          specialist_menu_ids,
          units,
          variables,
          menus_xml,
          locales));

  return result;
}
} // namespace IODD
