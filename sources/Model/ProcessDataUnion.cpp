#include "ProcessDataUnion.hpp"

using namespace std;

namespace IODD {
ProcessDataUnion::ProcessDataUnion(const string& id,
    ProcessDataTPtr&& in,
    ProcessDataTPtr&& out,
    const optional<Condition>& condition)
    : id_(id), in_(move(in)), out_(move(out)), condition_(condition) {
  if (!in_ && !out_) {
    throw invalid_argument("Neither ProcessDataIn nor ProcessDataOut is set. "
                           "Atleast one is required");
  }
}

string ProcessDataUnion::id() const { return id_; }

ProcessDataTPtr ProcessDataUnion::inData() const { return in_; }

ProcessDataTPtr ProcessDataUnion::outData() const { return out_; }

optional<Condition> ProcessDataUnion::condition() const { return condition_; }

}; // namespace IODD