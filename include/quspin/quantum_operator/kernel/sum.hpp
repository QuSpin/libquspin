#pragma once

#include <quspin/quantum_operator/quantum_operator.hpp>

namespace quspin {
  QuantumOperator sum(const QuantumOperator lhs, const QuantumOperator rhs);
}  // namespace quspin
