#include "Serializer.hpp"

#include <pugixml.hpp>

#include <filesystem>
#include <stdexcept>

using namespace std;
using namespace pugi;

namespace IODD {

IODevice decode(const xml_document& xml) {}

Repository deserializeModel(const string& directory_path) {
  Repository result;
  filesystem::path xml_dir = filesystem::path(directory_path).remove_filename();

  if (!filesystem::is_directory(xml_dir)) {
    throw invalid_argument(xml_dir.string() + " is not a directory");
  }

  for (const auto& entry : filesystem::directory_iterator(xml_dir)) {
    if (entry.is_regular_file() && entry.path().extension() == ".xml") {
      auto filename = entry.path().string();

      xml_document xml;
      if (auto status =
              xml.load_file(filename.c_str(), parse_default, encoding_utf8)) {
        result += decode(xml);
      } else {
        throw runtime_error("Failed to load " + filename +
            " as an XML document. " + status.description());
      }
    }
  }

  return Repository(result);
}
} // namespace IODD