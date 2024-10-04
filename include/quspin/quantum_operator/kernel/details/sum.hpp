#pragma once

#include <cstddef>
#include <cassert>
#include <quspin/quantum_operator/details/quantum_operator.hpp>

namespace quspin {
  namespace details {

    template<typename T, typename I, typename J>
    std::size_t get_sum_size(const quantum_operator<T, I, J> &lhs, const quantum_operator<T, I, J> &rhs) {
        assert(lhs.dim() == rhs.dim());
    }


  }  // namespace details

}  // namespace quspin