#ifndef __IODD_PROCESS_DATA_DECODER_HPP
#define __IODD_PROCESS_DATA_DECODER_HPP

#include "Datatypes.hpp"
#include "ProcessDataUnion.hpp"
#include "Variable.hpp"

#include <pugixml.hpp>

namespace IODD {
ProcessDataCollection decodeProcessData(const pugi::xml_node& xml,
    const pugi::xml_node& locales,
    const VariablesMap& variables,
    const DatatypesMapPtr& datatypes);
} // namespace IODD
#endif //__IODD_PROCESS_DATA_DECODER_HPP
