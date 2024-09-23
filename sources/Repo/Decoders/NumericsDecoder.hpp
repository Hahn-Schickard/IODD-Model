#ifndef __IODD_NUMERICS_DECODER_HPP
#define __IODD_NUMERICS_DECODER_HPP

#include "Model/Datatypes/Float.hpp"
#include "Model/Datatypes/Integer.hpp"
#include "Model/Datatypes/UInteger.hpp"

#include <pugixml.hpp>
#include <stdexcept>

using namespace std;

namespace IODD {
std::unordered_set<SingleValuePtr<bool>> decodeBoolSingleValues(
    const pugi::xml_node& node, const pugi::xml_node& locales);

NumberT<uint64_t>::SingleValues decodeUintSingleValues(
    const pugi::xml_node& node, const pugi::xml_node& locales);

NumberT<uint64_t>::ValueRanges decodeUintValueRanges(
    const pugi::xml_node& node, const pugi::xml_node& locales);

NumberT<int64_t>::SingleValues decodeIntSingleValues(
    const pugi::xml_node& node, const pugi::xml_node& locales);

NumberT<int64_t>::ValueRanges decodeIntValueRanges(
    const pugi::xml_node& node, const pugi::xml_node& locales);

NumberT<float>::SingleValues decodeFloatSingleValues(
    const pugi::xml_node& node, const pugi::xml_node& locales);

NumberT<float>::ValueRanges decodeFloatValueRanges(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <typename T>
NumberT<T> decodeNumericValues(
    const pugi::xml_node& /* node */, const pugi::xml_node& /* locales */) {
  throw runtime_error("Failed to decode Numeric Value. Unsupported data type");
}

template <>
NumberT<uint64_t> decodeNumericValues(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
NumberT<int64_t> decodeNumericValues(
    const pugi::xml_node& node, const pugi::xml_node& locales);

template <>
NumberT<float> decodeNumericValues(
    const pugi::xml_node& node, const pugi::xml_node& locales);
} // namespace IODD
#endif //__IODD_NUMERICS_DECODER_HPP