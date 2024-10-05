#pragma once

#include <cassert>
#include <cstddef>
#include <quspin/array/details/array.hpp>
#include <quspin/quantum_operator/details/quantum_operator.hpp>

namespace quspin {
  namespace details {

    template <typename T1, typename T2, typename I, typename J>
    std::size_t get_row_size(const I row_index, const quantum_operator<T1, I, J> &lhs,
                             const quantum_operator<T2, I, J> &rhs) {
      I lhs_row_start = lhs.indptr(row_index);
      const I lhs_row_end = lhs.indptr(row_index + 1);

      I rhs_row_start = rhs.indptr(row_index);
      const I rhs_row_end = rhs.indptr(row_index + 1);

      while (lhs_row_start != lhs_row_end && rhs_row_start != rhs_row_end) {
        const I lhs_col = lhs.indices(lhs_row_start);
        const J lhs_cindex = lhs.cindices(lhs_row_start);

        const I rhs_col = rhs.indices(rhs_row_start);
        const J rhs_cindex = rhs.cindices(rhs_row_start);

        if (lhs_col < rhs_col) {
          ++lhs_row_start;
        } else if (lhs_col > rhs_col) {
          ++rhs_row_start;
        } else if (lhs_cindex > rhs_cindex) {
          ++lhs_row_start;
        } else if (lhs_cindex < rhs_cindex) {
          ++rhs_row_start;
        } else {
          ++lhs_row_start;
          ++rhs_row_start;
        }
        ++sum_size;
      }

      while (lhs_row_start != lhs_row_end) {
        ++lhs_row_start;
        ++sum_size;
      }

      while (rhs_row_start != rhs_row_end) {
        ++rhs_row_start;
        ++sum_size;
      }

      return sum_size;
    }

    template <typename T1, typename T2, typename I, typename J>
    void get_sum_size(const quantum_operator<T1, I, J> &lhs, const quantum_operator<T2, I, J> &rhs,
                      array<I> &out_indptr) {
      assert(lhs.dim() == rhs.dim());
      assert(lhs.dim() == out_indptr.size() - 1);

      std::size_t sum_size = 0;

      I *indptr = out_indptr.mut_data();
      indptr[0] = 0;

      for (std::size_t row_index = 0; row_index < lhs.dim(); ++row_index) {
        sum_size += get_row_size(row_index, lhs, rhs);
        indptr[row_index + 1] = static_cast<I>(sum_size);
      }
    }

    template <typename T, typename I, typename J>
    void get_sum_size(const quantum_operator<T, I, J> &lhs, const J coeff_index,
                      const array<I> &new_cols, array<I> &out_indptr) {
      assert(lhs.dim() == rhs.dim());
      assert(lhs.dim() == out_indptr.size() - 1);

      I *indptr = out_indptr.mut_data();
      const I *new_cols_ptr = new_cols.data();
      indptr[0] = 0;

      for (std::size_t row_index = 0; row_index < lhs.dim(); ++row_index) {
        I row_size = lhs.indptr(row_index + 1);

        for (I i = lhs.indptr(row_index); i < lhs.indptr(row_index + 1); ++i) {
          row_size += static_cast<I>(lhs.indices(i) == new_cols_ptr[row_index]
                                     && lhs.cindices(i) == coeff_index);
        }

        indptr[row_index + 1] = static_cast<I>(row_size);
      }
    }

    template <typename T1, typename T2, typename T3, typename I, typename J>
    void populate_row(const I row_index, const quantum_operator<T1, I, J> &lhs,
                      const quantum_operator<T2, I, J> &rhs, array<I> &out_indptr) {
      I lhs_row_start = lhs.indptr(row_index);
      const I lhs_row_end = lhs.indptr(row_index + 1);

      I rhs_row_start = rhs.indptr(row_index);
      const I rhs_row_end = rhs.indptr(row_index + 1);

      I out_row_start = out.indptr(row_index);

      I *out_indptr = out.indptr();
      I *out_indices = out.indices();
      J *out_cindices = out.cindices();
      T *out_data = out.data();

      while (lhs_row_start != lhs_row_end && rhs_row_start != rhs_row_end) {
        const I lhs_col = lhs.indices(lhs_row_start);
        const J lhs_cindex = lhs.cindices(lhs_row_start);

        const I rhs_col = rhs.indices(rhs_row_start);
        const J rhs_cindex = rhs.cindices(rhs_row_start);

        if (lhs_col < rhs_col) {
          out_indices[out_row_start] = lhs_col;
          out_cindices[out_row_start] = lhs_cindex;
          out_data[out_row_start] = static_cast<T3>(lhs.data(lhs_row_start));
          ++lhs_row_start;
        } else if (lhs_col > rhs_col) {
          out_indices[out_row_start] = rhs_col;
          out_cindices[out_row_start] = rhs_cindex;
          out_data[out_row_start] = static_cast<T3>(rhs.data(rhs_row_start));
          ++rhs_row_start;
        } else if (lhs_cindex > rhs_cindex) {
          out_indices[out_row_start] = lhs_col;
          out_cindices[out_row_start] = lhs_cindex;
          out_data[out_row_start] = static_cast<T3>(lhs.data(lhs_row_start));
          ++lhs_row_start;
        } else if (lhs_cindex < rhs_cindex) {
          out_indices[out_row_start] = rhs_col;
          out_cindices[out_row_start] = rhs_cindex;
          out_data[out_row_start] = static_cast<T3>(rhs.data(rhs_row_start));
          ++rhs_row_start;
        } else {
          out_indices[out_row_start] = lhs_col;
          out_cindices[out_row_start] = lhs_cindex;
          out_data[out_row_start] = (static_cast<T3>(lhs.data(lhs_row_start))
                                     + static_cast<T3>(rhs.data(rhs_row_start)));
          ++lhs_row_start;
          ++rhs_row_start;
        }
        ++out_row_start;
      }
    }

    template <typename Op, typename T1, typename T2, typename T3, typename I, typename J>
    void elementwise_binary_operation(const quantum_operator<T1, I, J> &lhs,
                                      const quantum_operator<T2, I, J> &rhs,
                                      quantum_operator<T3, I, J> &out, Op &&op) {
      assert(lhs.dim() == rhs.dim());
      assert(lhs.dim() == out.dim());

      for (std::size_t row_index = 0; row_index < lhs.dim(), ++row_index) {
        populate_row(row_index, lhs, rhs, out);
      }
    }
  }  // namespace details

}  // namespace quspin