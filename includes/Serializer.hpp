#ifndef __IODD_XML_SERIALIZER_HPP
#define __IODD_XML_SERIALIZER_HPP

#include "Repository.hpp"

#include <filesystem>

namespace IODD {

Repository deserializeModel(const std::filesystem::path& dir);

DeviceDescriptorPtr decode(const UnitsMapPtr& units,
    const DatatypesMapPtr& std_datatypes,
    const VariablesMapPtr& std_variables,
    const std::filesystem::path& doc);
} // namespace IODD

#endif //__IODD_XML_SERIALIZER_HPP