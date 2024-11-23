// Copyright 2024 Phillip Weinberg
#pragma once

#include <quspin/array/array.hpp>
#include <quspin/details/optional.hpp>

namespace quspin {

Array add(const Array &lhs, const Array &rhs,
          details::Optional<Array> out = details::Optional<Array>());

Scalar norm(const Array &array);

bool allclose(const Array &lhs, const Array &rhs, const double rtol = 1e-5,
              const double atol = 1e-8);

}  // namespace quspin
