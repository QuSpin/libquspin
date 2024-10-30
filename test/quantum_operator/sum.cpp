
#include <quspin/array/array.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/quantum_operator/kernel/sum.hpp>
#include <quspin/quantum_operator/quantum_operator.hpp>

using namespace quspin;

int main() {
  Array indptr({0, 2, 4, 6});
  Array indices({0, 1, 1, 2, 0, 2});
  Array cindices = Array({0, 1, 1, 1, 0, 0}).astype(UInt8);
  Array data({1.0, 2.0, 3.0, -3.0, -2.0, -1.0});

  QuantumOperator op(data, indptr, indices, cindices);
  auto res = sum(op, op);

  return 0;
}
