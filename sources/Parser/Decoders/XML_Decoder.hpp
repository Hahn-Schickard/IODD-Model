#ifndef __IODD_XML_DECODER_HPP
#define __IODD_XML_DECODER_HPP

#include "Model/DeviceDescriptor.hpp"

#include <filesystem>

namespace IODD {

std::pair<DatatypesMapPtr, VariablesMapPtr> decodeStdDefinitions(
    const std::filesystem::path& path);

DescriptorsMap decodeDescriptors(const UnitsMapPtr& units,
    const std::pair<DatatypesMapPtr, VariablesMapPtr>& variables,
    const std::filesystem::path& path);

DeviceDescriptorPtr decode(const UnitsMapPtr& units,
    const DatatypesMapPtr& std_datatypes,
    const VariablesMapPtr& std_variables,
    const std::filesystem::path& doc);
} // namespace IODD

#endif //__IODD_XML_DECODER_HPP