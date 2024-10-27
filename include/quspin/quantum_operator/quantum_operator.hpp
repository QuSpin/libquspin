#pragma once

#include <iostream>
#include <quspin/array/array.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/quantum_operator/details/quantum_operator.hpp>

namespace quspin {

  class QuantumOperator : public DTypeObject<details::quantum_operators> {
    static details::quantum_operators default_value() {
      return details::quantum_operators(details::quantum_operator<double, int32_t, uint8_t>());
    }

  public:
    QuantumOperator() : DTypeObject<details::quantum_operators>(default_value()) {}
    QuantumOperator(const QuantumOperator &op);
    QuantumOperator(const details::quantum_operators &op);
    template <typename T, typename I, typename J>
    QuantumOperator(const details::quantum_operator<T, I, J> &op);
    QuantumOperator(Array data, Array indptr, Array indices, Array cindices);
    ~QuantumOperator() { std::cout << "here" << std::endl; }

    std::size_t dim() const;

    Array indptr() const;
    Array indices() const;
    Array cindices() const;
    Array data() const;
  };

}  // namespace quspin
