#pragma once

#include <iostream>
#include <quspin/array/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/qmatrix/details/qmatrix.hpp>

namespace quspin {

  class QuantumOperator : public DTypeObject<details::qmatrixs> {
    static details::qmatrixs default_value() {
      return details::qmatrixs(details::qmatrix<double, int32_t, uint8_t>());
    }

  public:
    QuantumOperator() : DTypeObject<details::qmatrixs>(default_value()) {}
    QuantumOperator(const details::qmatrixs &op);
    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    QuantumOperator(const details::qmatrix<T, I, J> &op);
    QuantumOperator(Array data, Array indptr, Array indices, Array cindices);

    std::size_t dim() const;

    Array indptr() const;
    Array indices() const;
    Array cindices() const;
    Array data() const;
  };

}  // namespace quspin