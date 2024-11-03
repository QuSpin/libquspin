#pragma once

#include <cassert>
#include <cstddef>
#include <quspin/array/details/array.hpp>
#include <quspin/details/threading.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/qmatrix/details/qmatrix.hpp>
#include <thread>

namespace quspin {
  namespace details {

    template <typename Op, PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    struct row_sum_tasks {
      Op &&op;
      const qmatrix<T, I, J> lhs;
      const qmatrix<T, I, J> rhs;
      I *indptr;

    public:
      row_sum_tasks(Op &&op, const qmatrix<T, I, J> &lhs, const qmatrix<T, I, J> &rhs, I *indptr)
          : op(op), lhs(lhs), rhs(rhs), indptr(indptr) {}

      std::size_t size() const { return lhs.dim(); }

      void do_work(const std::size_t row_index, const std::size_t) {
        I sum_size = 0;

        I lhs_row_start = lhs.indptr_at(row_index);
        const I lhs_row_end = lhs.indptr_at(row_index + 1);

        I rhs_row_start = rhs.indptr_at(row_index);
        const I rhs_row_end = rhs.indptr_at(row_index + 1);

        while (lhs_row_start != lhs_row_end && rhs_row_start != rhs_row_end) {
          const I lhs_col = lhs.indices_at(lhs_row_start);
          const J lhs_cindex = lhs.cindices_at(lhs_row_start);

          const I rhs_col = rhs.indices_at(rhs_row_start);
          const J rhs_cindex = rhs.cindices_at(rhs_row_start);

          if (lhs_col < rhs_col) {
            ++lhs_row_start;
            ++sum_size;
          } else if (lhs_col > rhs_col) {
            ++rhs_row_start;
            ++sum_size;
          } else if (lhs_cindex > rhs_cindex) {
            ++lhs_row_start;
            ++sum_size;
          } else if (lhs_cindex < rhs_cindex) {
            ++rhs_row_start;
            ++sum_size;
          } else {
            const T result = op(lhs.data_at(lhs_row_start), rhs.data_at(rhs_row_start));

            if (result != T()) {
              ++sum_size;
            }
            ++lhs_row_start;
            ++rhs_row_start;
          }
        }

        while (lhs_row_start != lhs_row_end) {
          ++lhs_row_start;
          ++sum_size;
        }

        while (rhs_row_start != rhs_row_end) {
          ++rhs_row_start;
          ++sum_size;
        }

        indptr[row_index + 1] = sum_size;
      }
    };

    template <typename Op, PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    void elementwise_binop_size(Op &&op, const qmatrix<T, I, J> &lhs, const qmatrix<T, I, J> &rhs,
                                array<I> &out_indptr, const std::size_t num_threads = 0) {
      assert(lhs.dim() == rhs.dim());
      assert(lhs.dim() == out_indptr.size() - 1);

      I *indptr = out_indptr.mut_data();
      indptr[0] = 0;

      row_sum_tasks<Op, T, I, J> tasks(op, lhs, rhs, indptr);
      WorkQueue(tasks).run(Schedule::SequentialBlocks, num_threads);

      // cumulaive sum
      for (std::size_t row_index = 1; row_index <= lhs.dim(); ++row_index) {
        indptr[row_index] += indptr[row_index - 1];
      }
    }

    template <typename Op, PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    class populate_row_tasks {
      Op &&op;
      const qmatrix<T, I, J> lhs;
      const qmatrix<T, I, J> rhs;
      qmatrix<T, I, J> out;

    public:
      populate_row_tasks(Op &&op, const qmatrix<T, I, J> &lhs, const qmatrix<T, I, J> &rhs,
                         qmatrix<T, I, J> &out)
          : op(op), lhs(lhs), rhs(rhs), out(out) {}

      std::size_t size() const { return lhs.dim(); }

      void do_work(const std::size_t row_index, const std::size_t) {
        I lhs_row_start = lhs.indptr_at(row_index);
        const I lhs_row_end = lhs.indptr_at(row_index + 1);

        I rhs_row_start = rhs.indptr_at(row_index);
        const I rhs_row_end = rhs.indptr_at(row_index + 1);

        I out_row_start = out.indptr_at(row_index);

        I *out_indices = out.indices_ptr();
        J *out_cindices = out.cindices_ptr();
        T *out_data = out.data_ptr();

        while (lhs_row_start != lhs_row_end && rhs_row_start != rhs_row_end) {
          const I lhs_col = lhs.indices_at(lhs_row_start);
          const J lhs_cindex = lhs.cindices_at(lhs_row_start);

          const I rhs_col = rhs.indices_at(rhs_row_start);
          const J rhs_cindex = rhs.cindices_at(rhs_row_start);

          if (lhs_col < rhs_col) {
            out_indices[out_row_start] = lhs_col;
            out_cindices[out_row_start] = lhs_cindex;
            out_data[out_row_start] = lhs.data_at(lhs_row_start);
            ++lhs_row_start;
            ++out_row_start;

          } else if (lhs_col > rhs_col) {
            out_indices[out_row_start] = rhs_col;
            out_cindices[out_row_start] = rhs_cindex;
            out_data[out_row_start] = rhs.data_at(rhs_row_start);
            ++rhs_row_start;
            ++out_row_start;

          } else if (lhs_cindex > rhs_cindex) {
            out_indices[out_row_start] = lhs_col;
            out_cindices[out_row_start] = lhs_cindex;
            out_data[out_row_start] = lhs.data_at(lhs_row_start);
            ++lhs_row_start;
            ++out_row_start;

          } else if (lhs_cindex < rhs_cindex) {
            out_indices[out_row_start] = rhs_col;
            out_cindices[out_row_start] = rhs_cindex;
            out_data[out_row_start] = rhs.data_at(rhs_row_start);
            ++rhs_row_start;
            ++out_row_start;

          } else {
            const T result = op(lhs.data_at(lhs_row_start), rhs.data_at(rhs_row_start));

            if (result != T()) {
              out_indices[out_row_start] = lhs_col;
              out_cindices[out_row_start] = lhs_cindex;
              out_data[out_row_start] = result;
              ++out_row_start;
            }
            ++lhs_row_start;
            ++rhs_row_start;
          }
        }
      }
    };

    template <typename Op, PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    void elementwise_binary_operation(Op &&op, const qmatrix<T, I, J> &lhs,
                                      const qmatrix<T, I, J> &rhs, qmatrix<T, I, J> &out,
                                      const std::size_t num_threads = 0) {
      static_assert(std::is_invocable_v<Op, T, T>, "Incompatible Operator with input types");
      static_assert(std::is_same_v<T, std::decay_t<std::invoke_result_t<Op, T, T>>>,
                    "Incompatible output type");
      assert(lhs.dim() == rhs.dim());
      assert(lhs.dim() == out.dim());

      populate_row_tasks<Op, T, I, J> tasks(op, lhs, rhs, out);
      WorkQueue(tasks).run(Schedule::SequentialBlocks, num_threads);
    }
  }  // namespace details
}  // namespace quspin
