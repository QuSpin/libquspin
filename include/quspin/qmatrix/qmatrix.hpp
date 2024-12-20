// Copyright 2024 Phillip Weinberg
#pragma once

#include <iostream>
#include <quspin/array/array.hpp>
#include <quspin/detail/type_concepts.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/qmatrix/detail/qmatrix.hpp>

namespace quspin {

class QMatrix : public DTypeObject<detail::qmatrices> {
    static detail::qmatrices default_value() {
      return detail::qmatrices(detail::qmatrix<double, int32_t, uint8_t>());
    }

  public:

    QMatrix() : DTypeObject<detail::qmatrices>(default_value()) {}

    QMatrix(const detail::qmatrices &op);
    template<PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    QMatrix(const detail::qmatrix<T, I, J> &op);
    QMatrix(Array data, Array indptr, Array indices, Array cindices);
    template<typename J>
      requires std::same_as<J, uint8_t> || std::same_as<J, uint16_t>
    QMatrix(Array data, Array indptr, Array indices, const J cindex);

    std::size_t dim() const;

    Array indptr() const;
    Array indices() const;
    Array cindices() const;
    Array data() const;
};

}  // namespace quspin
