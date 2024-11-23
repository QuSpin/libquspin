#pragma once

#include <quspin/qmatrix/qmatrix.hpp>

namespace quspin {
  QMatrix sum(const QMatrix lhs, const QMatrix rhs,
              const std::size_t num_threads = 0);
}  // namespace quspin
