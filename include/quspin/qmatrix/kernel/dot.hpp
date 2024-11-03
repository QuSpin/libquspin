#pragma once

#include <quspin/array/array.hpp>
#include <quspin/details/optional.hpp>
#include <quspin/qmatrix/qmatrix.hpp>

namespace quspin {
  Array dot(const QuantumOperator op, const Array coeff, const Array input,
            details::Optional<Array> output = details::Optional<Array>());
}  // namespace quspin
