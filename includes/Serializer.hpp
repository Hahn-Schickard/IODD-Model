#ifndef __IO_LINK_IODD_XML_SERIALIZER_HPP
#define __IO_LINK_IODD_XML_SERIALIZER_HPP

#include "Repository.hpp"

namespace IODD {

Repository deserializeModel(const std::string& directory_path);
} // namespace IODD

#endif //__IO_LINK_IODD_XML_SERIALIZER_HPP