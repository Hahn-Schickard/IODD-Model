#ifndef __IODD_STANDARD_DEFINES_PROCESS_DATA_UNION_T_HPP
#define __IODD_STANDARD_DEFINES_PROCESS_DATA_UNION_T_HPP

#include "Condition.hpp"
#include "Datatypes/ProcessData.hpp"

#include <memory>
#include <optional>

namespace IODD {

struct ProcessDataUnion {
  ProcessDataUnion(const std::string& id,
      ProcessDataTPtr&& in = nullptr,
      ProcessDataTPtr&& out = nullptr,
      const std::optional<Condition>& condition = std::nullopt);

  std::string id() const;

  ProcessDataTPtr inData() const;

  ProcessDataTPtr outData() const;

  std::optional<Condition> condition() const;

private:
  std::string id_;
  ProcessDataTPtr in_;
  ProcessDataTPtr out_;
  std::optional<Condition> condition_;
};

using ProcessDataUnionPtr = std::shared_ptr<ProcessDataUnion>;
using ProcessDataCollection =
    std::unordered_map<std::string, ProcessDataUnionPtr>;

}; // namespace IODD
#endif //__IODD_STANDARD_DEFINES_PROCESS_DATA_UNION_T_HPP