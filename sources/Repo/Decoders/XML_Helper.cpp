#include "XML_Helper.hpp"

#include <cstring>
#include <sstream>

using namespace std;
using namespace pugi;

xml_document getXML(const filesystem::path& path) {
  if (!filesystem::exists(path)) {
    throw runtime_error(path.string() + " does not exists");
  }

  if (path.extension() != ".xml") {
    throw invalid_argument(path.string() + " is not an XML document");
  }

  xml_document xml;
  if (auto status = xml.load_file(path.c_str())) {
    return xml;
  } else {
    throw runtime_error("Failed to load " + path.string() +
        " as an XML document. " + status.description());
  }
}

xml_node getXMLNode(const string& node_name,
    const xml_document& doc,
    const filesystem::path& path) {
  if (node_name.empty()) {
    throw invalid_argument("Could not get XML Node from XML doc. Node name "
                           "argument can not be empty");
  }

  auto node = doc.child(node_name.c_str());
  if (!node.empty()) {
    return node;
  } else {
    throw NodeNotFound(path, node_name);
  }
}

xml_node getXMLNode(const string& node_name,
    const xml_node& parent,
    const optional<filesystem::path>& path) {
  if (node_name.empty()) {
    throw invalid_argument(
        "Could not get XML Node from parent XML node. Node name "
        "argument can not be empty");
  }

  auto node = parent.child(node_name.c_str());
  if (!node.empty()) {
    return node;
  } else {
    if (path.has_value()) {
      throw NodeNotFound(path.value(), parent, node_name);
    } else {
      throw NodeNotFound(parent, node_name);
    }
  }
}

xml_node getXMLNode(const vector<string>& nodes,
    const xml_node& parent,
    const optional<filesystem::path>& path) {
  if (nodes.empty()) {
    throw invalid_argument(
        "Could not get XML Node from parent XML node. Nodes vector<string> "
        "argument can not be empty");
  }

  auto it = nodes.begin();
  auto node = getXMLNode(*it, parent, path);
  ++it;
  while (it != nodes.end()) {
    node = getXMLNode(*it, node, path);
    ++it;
  }

  return node;
}

xml_attribute getXMLAttribute(
    const string& attribute_name, const xml_node& node) {
  if (attribute_name.empty()) {
    throw invalid_argument("Could not get XML Node attribute " +
        string(node.name()) +
        " XML node. Attribute name argument can not be empty");
  }

  if (auto result = node.attribute(attribute_name.c_str()); !result.empty()) {
    return result;
  } else {
    throw AttributeNotFound(node, attribute_name);
  }
}

xml_attribute getXMLAttribute(const string& attribute_name,
    const vector<string>& nodes,
    const xml_node& parent) {
  if (attribute_name.empty()) {
    throw invalid_argument("Could not get XML Node attribute " +
        string(parent.name()) +
        " XML node. Attribute name argument can not be empty");
  }
  auto node = getXMLNode(nodes, parent);
  return getXMLAttribute(attribute_name, node);
}
