// Copyright 2024 Phillip Weinberg
#pragma once

#include <quspin/array/array.hpp>
#include <quspin/detail/optional.hpp>
#include <quspin/qmatrix/qmatrix.hpp>

namespace quspin {
Array dot(const bool overwrite_out, QMatrix op, Array coeff, Array input,
          detail::Optional<Array> output = detail::Optional<Array>());
}  // namespace quspin
