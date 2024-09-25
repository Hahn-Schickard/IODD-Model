#ifndef __IODD_CONDITION_DECODER_HPP
#define __IODD_CONDITION_DECODER_HPP

#include "Condition.hpp"
#include "Variable.hpp"

#include <optional>
#include <pugixml.hpp>

namespace IODD {
std::optional<Condition> decodeCondition(
    const pugi::xml_node& xml, const VariablesMapPtr& variables);
}

#endif //__IODD_CONDITION_DECODER_HPP