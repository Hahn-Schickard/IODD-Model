#ifndef __IODD_VARIABLES_DECODER_HPP
#define __IODD_VARIABLES_DECODER_HPP

#include "Model/Variable.hpp"

#include <pugixml.hpp>

namespace IODD {
VariablesMap decodeVariables(const pugi::xml_node& xml,
    const pugi::xml_node& locales,
    const DatatypesMap& datatypes,
    const VariablesMap& std_variables = {});

VariablesMap decodeStdVariables(const pugi::xml_node& xml,
    const pugi::xml_node& locales,
    const DatatypesMapPtr& datatypes,
    const VariablesMap& std_variables);
} // namespace IODD
#endif //__IODD_VARIABLES_DECODER_HPP