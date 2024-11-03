
#include <quspin/array/array.hpp>
#include <quspin/array/kernels.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/qmatrix/kernel/sum.hpp>
#include <quspin/qmatrix/qmatrix.hpp>

using namespace quspin;

void test_1() {
  Array indptr({0, 2, 4, 6});
  Array indices({0, 1, 1, 2, 0, 2});
  Array cindices = Array({0, 1, 1, 1, 0, 0}).astype(UInt8);
  Array data({1.0, 2.0, 3.0, -3.0, -2.0, -1.0});

  QuantumOperator op(data, indptr, indices, cindices);
  auto res = sum(op, op);

  assert(res.data().shape() == data.shape());
  assert(res.indptr().shape() == indptr.shape());
  assert(res.indices().shape() == indices.shape());
  assert(res.cindices().shape() == cindices.shape());

  // expect 2 * data
  Array expected_data({2.0, 4.0, 6.0, -6.0, -4.0, -2.0});

  assert(allclose(res.data(), expected_data));
  assert(allclose(res.indptr(), indptr));
  assert(allclose(res.indices(), indices));
  assert(allclose(res.cindices(), cindices));
}

void test_2() {}

int main() {
  test_1();

  return 0;
}
