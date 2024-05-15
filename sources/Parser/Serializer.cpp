#include "Serializer.hpp"

#include <pugixml.hpp>

#include <filesystem>
#include <stdexcept>

using namespace std;
using namespace pugi;

namespace IODD {

IODeviceDescriptorPtr decode(const xml_document& xml) {}

xml_document getXML(const filesystem::path& path) {
  if (!filesystem::exists(path)) {
    throw runtime_error(path.string() + " does not exists");
  }

  if (path.extension() != ".xml") {
    throw invalid_argument(path.string() + " is not an XML document");
  }

  xml_document xml;
  if (auto status = xml.load_file(path.c_str(), parse_default, encoding_utf8)) {
    return xml;
  } else {
    throw runtime_error("Failed to load " + path.string() +
        " as an XML document. " + status.description());
  }
}

Repository::UnitsMap decodeUnits(const filesystem::path& path) {
  auto unit_xml = getXML(path);
}

Repository::VariablesMap decodeStdVariables(const filesystem::path& path) {
  auto variables_xml = getXML(path);
}

Repository::DescriptorsMap decodeDescriptors(const filesystem::path& path) {
  Repository::DescriptorsMap descriptors;

  for (const auto& entry : filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".xml") {
      xml_document descriptor_xml = getXML(entry.path());
      auto descriptor = decode(descriptor_xml);
      descriptors.emplace(descriptor->getDeviceIdentity(), descriptor);
    }
  }
  return descriptors;
}

Repository deserializeModel(const string& config_directory_path) {
  auto config_dir = filesystem::path(config_directory_path);

  if (!filesystem::is_directory(config_dir)) {
    throw invalid_argument(config_dir.string() + " is not a directory");
  }

  auto descriptors_dir = config_dir / "descriptors";
  auto std_units_filepath = config_dir / "IODD-StandardDefinitions1.1.xml";
  auto std_variables_filepath =
      config_dir / "IODD-StandardUnitDefinitions1.1.1.xml";
  return Repository(decodeDescriptors(descriptors_dir),
      decodeUnits(std_units_filepath),
      decodeStdVariables(std_variables_filepath));
}
} // namespace IODD