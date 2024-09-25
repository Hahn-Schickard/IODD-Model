#ifndef __IODD_UI_DECODER_HPP
#define __IODD_UI_DECODER_HPP

#include "UserInterface.hpp"

#include <pugixml.hpp>
#include <unordered_map>

namespace IODD {
std::unordered_map<UserRole, UserInterfacePtr> decodeUI(
    const UnitsMapPtr& units,
    const VariablesMap& variables,
    const pugi::xml_node& xml,
    const pugi::xml_node& locales);
}
#endif //__IODD_UI_DECODER_HPP