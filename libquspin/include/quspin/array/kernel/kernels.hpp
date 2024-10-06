#pragma once

#include <quspin/array/array.hpp>
#include <quspin/details/optional.hpp>

namespace quspin {

  Array add(const Array &lhs, const Array &rhs,
            details::Optional<Array> out = details::Optional<Array>());

  Scalar norm(const Array &array);

}  // namespace quspin
