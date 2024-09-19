#ifndef __IODD_PRIMITIES_DECODER_HPP
#define __IODD_PRIMITIES_DECODER_HPP

#include "Model/Datatypes/AccessRights.hpp"
#include "Model/Datatypes/Primitives/TextID.hpp"

#include <optional>
#include <pugixml.hpp>

namespace IODD {
TextID decodeLocalization(
    const pugi::xml_node& locales, const std::string& text_id);

std::optional<AccessRights> decodeAccessRights(const pugi::xml_node& node);

std::optional<TextID> decodeLocalizedText(const std::string& child_name,
    const pugi::xml_node& node,
    const pugi::xml_node& locales);
} // namespace IODD
#endif //__IODD_PRIMITIES_DECODER_HPP
