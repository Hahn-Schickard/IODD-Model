#ifndef __IODD_XML_HELPER_HPP
#define __IODD_XML_HELPER_HPP

#include <pugixml.hpp>

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

struct NodeNotFound : public std::runtime_error {
  NodeNotFound(const pugi::xml_document& doc, const std::string& name)
      : runtime_error(
            std::string(doc.name()) + " does not have " + name + " xml node") {}

  NodeNotFound(const std::filesystem::path& path, const std::string& name)
      : runtime_error(path.string() + " does not have " + name + " xml node") {}

  NodeNotFound(const std::filesystem::path& path,
      const pugi::xml_node& parent,
      const std::string& name)
      : runtime_error(path.string() + "  Doc parent node " +
            std::string(parent.name()) + " does not have " + name +
            " xml node") {}

  NodeNotFound(const pugi::xml_node& parent, const std::string& name)
      : runtime_error("Parent node " + std::string(parent.name()) +
            " does not have " + name + " xml node") {}
};

struct AttributeNotFound : public std::runtime_error {
  AttributeNotFound(const pugi::xml_node& node, const std::string& name)
      : runtime_error("XML Node " + std::string(node.name()) +
            " does not have " + name + " attribute") {}
};

pugi::xml_document getXML(const std::filesystem::path& path);

// assumes doc is not empty
pugi::xml_node getXMLNode(const std::string& node_name,
    const pugi::xml_document& doc,
    const std::filesystem::path& path);

// assumes parent is not empty
pugi::xml_node getXMLNode(const std::string& node_name,
    const pugi::xml_node& parent,
    const std::optional<std::filesystem::path>& path = std::nullopt);

// assumes parent is not empty
pugi::xml_node getXMLNode(const std::vector<std::string>& nodes,
    const pugi::xml_node& parent,
    const std::optional<std::filesystem::path>& path = std::nullopt);

// assumes node is not empty
pugi::xml_attribute getXMLAttribute(
    const std::string& attribute_name, const pugi::xml_node& node);

// assumes parent is not empty
pugi::xml_attribute getXMLAttribute(const std::string& attribute_name,
    const std::vector<std::string>& nodes,
    const pugi::xml_node& parent);

#endif //__IODD_XML_HELPER_HPP
