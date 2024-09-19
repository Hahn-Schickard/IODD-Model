#include "NumericsDecoder.hpp"
#include "PrimitivesDecoder.hpp"
#include "XML_Helper.hpp"

using namespace std;
using namespace pugi;

namespace IODD {
unordered_set<SingleValuePtr<bool>> decodeBoolSingleValues(
    const xml_node& node, const xml_node& locales) {
  unordered_set<SingleValuePtr<bool>> result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<bool>>(
        getXMLAttribute("value", node_value).as_bool(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<uint64_t>::SingleValues decodeUintSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<uint64_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<uint64_t>>(
        getXMLAttribute("value", node_value).as_ullong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<uint64_t>::ValueRanges decodeUintValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<uint64_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<uint64_t>>(
        getXMLAttribute("lowerValue", node_value).as_ullong(),
        getXMLAttribute("upperValue", node_value).as_ullong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<int64_t>::SingleValues decodeIntSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<int64_t>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<int64_t>>(
        getXMLAttribute("value", node_value).as_llong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<int64_t>::ValueRanges decodeIntValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<int64_t>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<int64_t>>(
        getXMLAttribute("lowerValue", node_value).as_llong(),
        getXMLAttribute("upperValue", node_value).as_llong(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<float>::SingleValues decodeFloatSingleValues(
    const xml_node& node, const xml_node& locales) {
  NumberT<float>::SingleValues result;
  for (auto node_value : node.children("SingleValue")) {
    result.emplace(make_shared<SingleValue<float>>(
        getXMLAttribute("value", node_value).as_float(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

NumberT<float>::ValueRanges decodeFloatValueRanges(
    const xml_node& node, const xml_node& locales) {
  NumberT<float>::ValueRanges result;
  for (auto node_value : node.children("ValueRange")) {
    result.emplace(make_shared<ValueRange<float>>(
        getXMLAttribute("lowerValue", node_value).as_float(),
        getXMLAttribute("upperValue", node_value).as_float(),
        decodeLocalizedText("Name", node_value, locales)));
  }
  return result;
}

template <>
NumberT<uint64_t> decodeNumericValues(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeUintSingleValues(node, locales);
  auto ranges = decodeUintValueRanges(node, locales);
  return NumberT(move(values), move(ranges));
}

template <>
NumberT<int64_t> decodeNumericValues(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeIntSingleValues(node, locales);
  auto ranges = decodeIntValueRanges(node, locales);
  return NumberT(move(values), move(ranges));
}

template <>
NumberT<float> decodeNumericValues(
    const xml_node& node, const xml_node& locales) {
  auto values = decodeFloatSingleValues(node, locales);
  auto ranges = decodeFloatValueRanges(node, locales);
  return NumberT(move(values), move(ranges));
}
} // namespace IODD
