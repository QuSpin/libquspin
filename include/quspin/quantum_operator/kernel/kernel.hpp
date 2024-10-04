#pragma once

#include <cstddef>
#include <quspin/array/array.hpp>
#include <quspin/quantum_operator/quantum_operator.hpp>

namespace quspin {

  std::size_t get_sum_size(const QuantumOperator &lhs, const QuantumOperator &rhs);
  void sum(const QuantumOperator &lhs, const QuantumOperator &rhs, QuantumOperator &out);
  void dot(const QuantumOperator &op, const Array &coeff, const Array &input, Array &output);

}  // namespace quspin
