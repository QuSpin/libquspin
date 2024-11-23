// Copyright 2024 Phillip Weinberg
#pragma once

#include <cassert>
#include <cstddef>
#include <quspin/array/details/array.hpp>
#include <quspin/details/threading.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/qmatrix/details/qmatrix.hpp>
#include <thread>

namespace quspin { namespace details {

template<typename Op, PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
void elementwise_binop_size(Op &&op, const qmatrix<T, I, J> &lhs,
                            const qmatrix<T, I, J> &rhs, array<I> &out_indptr,
                            const std::size_t num_threads = 0) {
  assert(lhs.dim() == rhs.dim());
  assert(lhs.dim() == out_indptr.size() - 1);

  I *indptr = out_indptr.mut_data();
  indptr[0] = 0;

#pragma omp parallel for num_threads(num_threads)
  for (std::size_t row_index = 0; row_index < lhs.dim(); ++row_index) {
    I sum_size = 0;

    iterator<T, I, J> lhs_row_begin = lhs.row_begin(row_index);
    iterator<T, I, J> lhs_row_end = lhs.row_end(row_index);

    iterator<T, I, J> rhs_row_begin = rhs.row_begin(row_index);
    iterator<T, I, J> rhs_row_end = rhs.row_end(row_index);

    while (lhs_row_begin != lhs_row_end && rhs_row_begin != rhs_row_end) {
      if (*lhs_row_begin < *rhs_row_begin) {
        ++lhs_row_begin;
        ++sum_size;
      } else if (*lhs_row_begin > *rhs_row_begin) {
        ++rhs_row_begin;
        ++sum_size;
      } else {
        const T result = op(lhs_row_begin.value(), rhs_row_begin.value());

        if (result != T()) {
          ++sum_size;
        }
        ++lhs_row_begin;
        ++rhs_row_begin;
      }
    }
    indptr[row_index + 1] = cast<I>(sum_size + (lhs_row_end - lhs_row_begin) +
                                    (rhs_row_end - rhs_row_begin));
  }

  // cumulaive sum
  for (std::size_t row_index = 1; row_index <= lhs.dim(); ++row_index) {
    indptr[row_index] += indptr[row_index - 1];
  }
}

template<typename Op, PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
void elementwise_binary_operation(Op &&op, const qmatrix<T, I, J> &lhs,
                                  const qmatrix<T, I, J> &rhs,
                                  qmatrix<T, I, J> &out,
                                  const std::size_t num_threads = 0) {
  static_assert(std::is_invocable_v<Op, T, T>,
                "Incompatible Operator with input types");
  static_assert(std::is_same_v<T, std::decay_t<std::invoke_result_t<Op, T, T>>>,
                "Incompatible output type");
  assert(lhs.dim() == rhs.dim());
  assert(lhs.dim() == out.dim());

#pragma omp parallel for num_threads(num_threads)
  for (std::size_t row_index = 0; row_index < lhs.dim(); ++row_index) {
    auto lhs_row_begin = lhs.row_begin(row_index);
    auto lhs_row_end = lhs.row_end(row_index);
    auto rhs_row_begin = rhs.row_begin(row_index);
    auto rhs_row_end = rhs.row_end(row_index);

    auto out_indices = out.indices_ptr();
    auto out_cindices = out.cindices_ptr();
    auto out_data = out.data_ptr();

    std::size_t out_row_offset = out.indptr_at(row_index);

    while (lhs_row_begin != lhs_row_end && rhs_row_begin != rhs_row_end) {
      if (*lhs_row_begin < *rhs_row_begin) {
        out_indices[out_row_offset] = lhs_row_begin.index();
        out_cindices[out_row_offset] = lhs_row_begin.cindex();
        out_data[out_row_offset] = lhs_row_begin.value();
        ++lhs_row_begin;
        ++out_row_offset;

      } else if (*lhs_row_begin > *rhs_row_begin) {
        out_indices[out_row_offset] = rhs_row_begin.index();
        out_cindices[out_row_offset] = rhs_row_begin.cindex();
        out_data[out_row_offset] = rhs_row_begin.value();
        ++rhs_row_begin;
        ++out_row_offset;

      } else {
        const T result = op(lhs_row_begin.value(), rhs_row_begin.value());

        if (result != T()) {
          out_indices[out_row_offset] = lhs_row_begin.index();
          out_cindices[out_row_offset] = lhs_row_begin.cindex();
          out_data[out_row_offset] = result;
          ++out_row_offset;
        }
        ++lhs_row_begin;
        ++rhs_row_begin;
      }
    }
  }
}
}}  // namespace quspin::details
