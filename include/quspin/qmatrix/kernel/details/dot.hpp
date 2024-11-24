// Copyright 2024 Phillip Weinberg
#pragma once

#include <concepts>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/qmatrix/details/qmatrix.hpp>
#include <type_traits>

namespace quspin {
namespace details {

template<typename T, typename I, typename J, typename K>
concept QMatrixDotTypes =
    QMatrixTypes<T, I, J> &&
    std::same_as<std::decay_t<K>, std::common_type_t<T, J>>;

template<typename T, typename I, typename J, typename K>
  requires QMatrixDotTypes<T, I, J, K>
void dot_1d(const qmatrix<T, I, J> qmat, const array<K> coeffs,
            const array<K> vec, array<K> out) {
  assert(qmat.dim() == vec.size());
  assert(qmat.dim() == out.size());

  for (std::size_t row_index = 0; row_index < qmat.dim(); ++row_index) {
    std::array<K, qmatrix<T, I, J>::max_coeff> sums;
    std::fill_n(sums.begin(), coeffs.size(), K());

    auto qmat_row_begin = qmat.row_begin(row_index);
    auto qmat_row_end = qmat.row_end(row_index);

    for (auto qmat_iter = qmat_row_begin; qmat_iter != qmat_row_end;
         ++qmat_iter) {
      sums[qmat_iter.cindex()] +=
          qmat_iter.value() * vec.at({qmat_iter.index()});
    }

    out.at({row_index}) =
        std::inner_product(coeffs.begin(), coeffs.end(), sums.begin(), K());
  }
}
