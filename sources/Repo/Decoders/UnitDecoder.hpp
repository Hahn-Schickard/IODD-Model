#ifndef __IODD_UNIT_DECODER_HPP
#define __IODD_UNIT_DECODER_HPP

#include "Unit.hpp"

#include <filesystem>
#include <pugixml.hpp>

namespace IODD {
UnitsMapPtr decodeUnits(const std::filesystem::path& path);

UnitPtr decodeUnitPtr(const UnitsMapPtr& units, const pugi::xml_node& xml);
} // namespace IODD

#endif //__IODD_UNIT_DECODER_HPP