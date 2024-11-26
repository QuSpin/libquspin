// Copyright 2024 Phillip Weinberg
#pragma once

#include <concepts>
#include <numeric>
#include <quspin/array/detail/array.hpp>
#include <quspin/detail/type_concepts.hpp>
#include <quspin/qmatrix/detail/qmatrix.hpp>
#include <type_traits>

namespace quspin { namespace detail {

template<typename T, typename I, typename J, typename coeff_t, typename vec_t,
         typename out_t>
concept QMatrixDotTypes =
    QMatrixTypes<T, I, J> &&
    std::same_as<std::decay_t<out_t>, upcast_t<T, coeff_t, vec_t>>;

template<typename T, typename I, typename J, typename coeff_t, typename vec_t,
         typename out_t>
  requires QMatrixDotTypes<T, I, J, coeff_t, vec_t, out_t>
void check_args(const qmatrix<T, I, J> &qmat, const array<coeff_t> &coeffs,
                const array<vec_t> &vec, array<out_t> &out) {
  if (qmat.dim() != out.shape(0)) {
    throw std::invalid_argument("Output array has incorrect shape");
  }

  if (qmat.num_coeff() != coeffs.size()) {
    throw std::invalid_argument("Coefficient array has incorrect size");
  }

  if (vec.shape(0) != qmat.dim()) {
    throw std::invalid_argument("Vector array has incorrect size");
  }

  if (vec.shape() != out.shape()) {
    throw std::invalid_argument(
        "Vector and output arrays have different shapes");
  }
}

template<typename T, typename I, typename J, typename coeff_t, typename vec_t,
         typename out_t>
  requires QMatrixDotTypes<T, I, J, coeff_t, vec_t, out_t>
void dot_1d(const bool overwrite_out, const qmatrix<T, I, J> &qmat,
            const array<coeff_t> &coeffs, const array<vec_t> &vec,
            array<out_t> &out) {
  check_args(qmat, coeffs, vec, out);

  using index_type = std::array<std::size_t, 1>;

  for (std::size_t row_index = 0; row_index < qmat.dim(); ++row_index) {
    std::array<out_t, qmatrix<T, I, J>::max_coeff> sums;
    std::fill_n(sums.begin(), coeffs.size(), out_t());

    for (auto qmat_iter = qmat.row_begin(row_index);
         qmat_iter != qmat.row_end(row_index); ++qmat_iter) {
      const std::size_t index = qmat_iter.index();
      sums[qmat_iter.cindex()] += qmat_iter.value() * vec[{index}];
    }

    out[index_type{row_index}] = std::inner_product(
        sums.begin(), sums.end(), coeffs.begin(),
        overwrite_out ? out[index_type{row_index}] : out_t());
  }
}

template<typename T, typename I, typename J, typename coeff_t, typename vec_t,
         typename out_t>
  requires QMatrixDotTypes<T, I, J, coeff_t, vec_t, out_t>
void dot_2d(const bool overwrite_out, const qmatrix<T, I, J> &qmat,
            const array<coeff_t> &coeffs, const array<vec_t> &vec,
            array<out_t> &out) {
  check_args(qmat, coeffs, vec, out);

  using index_type = std::array<std::size_t, 2>;

  for (std::size_t row_index = 0; row_index < qmat.dim(); ++row_index) {
    const auto qmat_end = qmat.row_end(row_index);
    if (vec.strides(0) < vec.strides(1)) {
      for (std::size_t col_index = 0; col_index < vec.shape(1); ++col_index) {
        for (auto qmat_iter = qmat.row_begin(row_index); qmat_iter != qmat_end;
             ++qmat_iter) {
          const T value = qmat_iter.value();
          const std::size_t index = qmat_iter.index();
          const std::size_t cindex = qmat_iter.cindex();
          const coeff_t coeff = coeffs[std::array<std::size_t, 1>{cindex}];

          index_type out_index = {row_index, col_index};
          index_type vec_index = {index, col_index};

          out[out_index] = value * vec[vec_index] * coeff +
                           (overwrite_out ? out[out_index] : out_t());
        }
      }
    } else {
      for (auto qmat_iter = qmat.row_begin(row_index); qmat_iter != qmat_end;
           ++qmat_iter) {
        const T value = qmat_iter.value();
        const std::size_t index = qmat_iter.index();
        const std::size_t cindex = qmat_iter.cindex();
        const coeff_t coeff = coeffs[std::array<std::size_t, 1>{cindex}];

        for (std::size_t col_index = 0; col_index < vec.shape(1); ++col_index) {
          index_type out_index = {row_index, col_index};
          index_type vec_index = {index, col_index};

          out[out_index] = value * vec[vec_index] * coeff +
                           (overwrite_out ? out[out_index] : out_t());
        }
      }
    }
  }
}

template<typename T, typename I, typename J, typename coeff_t, typename vec_t,
         typename out_t>
  requires QMatrixDotTypes<T, I, J, coeff_t, vec_t, out_t>
void dot(const bool overwrite_out, const qmatrix<T, I, J> &qmat,
         const array<coeff_t> &coeffs, const array<vec_t> &vec,
         array<out_t> &out) {
  if (vec.ndim() == 1) {
    dot_1d(overwrite_out, qmat, coeffs, vec, out);
  } else if (vec.ndim() == 2) {
    dot_2d(overwrite_out, qmat, coeffs, vec, out);
  } else {
    throw std::invalid_argument(
        "Vector array has incorrect number of dimensions");
  }
}

}}  // namespace quspin::detail

void test_compile() {
  using namespace quspin::detail;
  using T = double;
  using I = int32_t;
  using J = uint8_t;
  using K = double;

  array<T> data({1, 2, 3, 4, 5, 6, 7, 8, 9});
  array<I> indptr({0, 3, 6, 9});
  array<I> indices({0, 1, 2, 0, 1, 2, 0, 1, 2});
  array<J> cindices({0, 1, 2, 0, 1, 2, 0, 1, 2});

  qmatrix<T, I, J> qmat(3, data, indptr, indices, cindices);
  array<K> coeffs({1, 2, 3});
  array<K> vec({1, 2, 3});
  array<K> out({1, 2, 3});
  dot(true, qmat, coeffs, vec, out);
}
