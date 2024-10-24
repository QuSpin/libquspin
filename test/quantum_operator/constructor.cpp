
#include <cassert>
#include <exception>
#include <quspin/array/array.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/quantum_operator/quantum_operator.hpp>
#include <string>

using namespace quspin;

int main() {
  Array indptr({4}, Int32);
  Array indices({8}, Int32);
  Array cindices({8}, UInt8);
  Array data({8}, Double);

  for (std::size_t i = 1; i < indptr.shape(0); i++) {
    indptr[{i}] = 2;
  }

  for (std::size_t i = 1; i < indptr.shape(0) - 1; i++) {
    indptr[{i}] = indptr[{i - 1}] + indptr[{i}];
  }

  return 0;
}
