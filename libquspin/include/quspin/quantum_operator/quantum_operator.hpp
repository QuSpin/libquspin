#pragma once

#include <quspin/dtype/dtype.hpp>
#include <quspin/quantum_operator/details/quantum_operator.hpp>

namespace quspin {

  class QuantumOperator : public DTypeObject<details::quantum_operators> {
    using DTypeObject<details::quantum_operators>::internals_;

  public:
    QuantumOperator(const QuantumOperator &op);
    QuantumOperator(const details::quantum_operators &op);
    template <typename T, typename I, typename J>
    QuantumOperator(const details::quantum_operator<T, I, J> &op);
    QuantumOperator(Array, Array, Array, Array);
  };

}  // namespace quspin