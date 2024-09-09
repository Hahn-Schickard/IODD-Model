#ifndef __IODD_XML_SERIALIZER_HPP
#define __IODD_XML_SERIALIZER_HPP

#include "Repository.hpp"

namespace IODD {

Repository deserializeModel(const std::string& directory_path);
} // namespace IODD

#endif //__IODD_XML_SERIALIZER_HPP