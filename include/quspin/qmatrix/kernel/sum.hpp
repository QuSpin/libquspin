#pragma once

#include <quspin/qmatrix/qmatrix.hpp>

namespace quspin {
  QuantumOperator sum(const QuantumOperator lhs, const QuantumOperator rhs,
                      const std::size_t num_threads = 0);
}  // namespace quspin
