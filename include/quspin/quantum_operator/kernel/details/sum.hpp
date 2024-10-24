#pragma once

#include <cassert>
#include <cstddef>
#include <quspin/array/details/array.hpp>
#include <quspin/details/threading.hpp>
#include <quspin/quantum_operator/details/quantum_operator.hpp>
#include <thread>

namespace quspin {
  namespace details {

    template <typename Op, typename T1, typename T2, typename I, typename J> struct row_sum_tasks {
      Op &&op;
      const quantum_operator<T1, I, J> lhs;
      const quantum_operator<T2, I, J> rhs;
      I *indptr;

    public:
      row_sum_tasks(Op &&op, const quantum_operator<T1, I, J> &lhs,
                    const quantum_operator<T2, I, J> &rhs, I *indptr)
          : op(op), lhs(lhs), rhs(rhs), indptr(indptr) {}

      std::size_t size() const { return lhs.dim(); }

      void do_work(const std::size_t row_index) {
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
            auto result = op(lhs.data_at(lhs_row_start), rhs.data_at(rhs_row_start));
            if (result != 0) {
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

    template <typename Op, typename T1, typename T2, typename I, typename J>
    void get_sum_size(Op &&op, const quantum_operator<T1, I, J> &lhs,
                      const quantum_operator<T2, I, J> &rhs, array<I> &out_indptr) {
      assert(lhs.dim() == rhs.dim());
      assert(lhs.dim() == out_indptr.size() - 1);

      I *indptr = out_indptr.mut_data();
      indptr[0] = 0;

      WorkQueue(row_sum_tasks(op, lhs, rhs, indptr))
          .run(Schedule::SequentialBlocks, std::thread::hardware_concurrency());

      // cumulaive sum
      for (std::size_t row_index = 1; row_index <= lhs.dim(); ++row_index) {
        indptr[row_index] += indptr[row_index - 1];
      }
    }

    template <typename Op, typename T1, typename T2, typename T3, typename I, typename J>
    class populate_row_tasks {
      Op &&op;
      const quantum_operator<T1, I, J> lhs;
      const quantum_operator<T2, I, J> rhs;
      quantum_operator<T3, I, J> out;

      populate_row_tasks(Op &&op, const quantum_operator<T1, I, J> &lhs,
                         const quantum_operator<T2, I, J> &rhs, quantum_operator<T3, I, J> &out)
          : op(op), lhs(lhs), rhs(rhs), out(out) {}

    public:
      std::size_t size() const { return lhs.dim(); }

      void do_work(const std::size_t row_index) {
        I lhs_row_start = lhs.indptr_at(row_index);
        const I lhs_row_end = lhs.indptr_at(row_index + 1);

        I rhs_row_start = rhs.indptr_at(row_index);
        const I rhs_row_end = rhs.indptr_at(row_index + 1);

        I out_row_start = out.indptr_at(row_index);

        I *out_indptr = out.indptr_ptr();
        I *out_indices = out.indices_ptr();
        J *out_cindices = out.cindices_ptr();
        T3 *out_data = out.data_ptr();

        while (lhs_row_start != lhs_row_end && rhs_row_start != rhs_row_end) {
          const I lhs_col = lhs.indices_at(lhs_row_start);
          const J lhs_cindex = lhs.cindices_at(lhs_row_start);

          const I rhs_col = rhs.indices_at(rhs_row_start);
          const J rhs_cindex = rhs.cindices_at(rhs_row_start);

          if (lhs_col < rhs_col) {
            out_indices[out_row_start] = lhs_col;
            out_cindices[out_row_start] = lhs_cindex;
            out_data[out_row_start] = static_cast<T3>(lhs.data_at(lhs_row_start));
            ++lhs_row_start;
            ++out_row_start;

          } else if (lhs_col > rhs_col) {
            out_indices[out_row_start] = rhs_col;
            out_cindices[out_row_start] = rhs_cindex;
            out_data[out_row_start] = static_cast<T3>(rhs.data_at(rhs_row_start));
            ++rhs_row_start;
            ++out_row_start;

          } else if (lhs_cindex > rhs_cindex) {
            out_indices[out_row_start] = lhs_col;
            out_cindices[out_row_start] = lhs_cindex;
            out_data[out_row_start] = static_cast<T3>(lhs.data_at(lhs_row_start));
            ++lhs_row_start;
            ++out_row_start;

          } else if (lhs_cindex < rhs_cindex) {
            out_indices[out_row_start] = rhs_col;
            out_cindices[out_row_start] = rhs_cindex;
            out_data[out_row_start] = static_cast<T3>(rhs.data_at(rhs_row_start));
            ++rhs_row_start;
            ++out_row_start;

          } else {
            auto result = op(lhs.data_at(lhs_row_start), rhs.data_at(rhs_row_start));
            if (result != 0) {
              out_indices[out_row_start] = lhs_col;
              out_cindices[out_row_start] = lhs_cindex;
              out_data[out_row_start] = static_cast<T3>(result);
              ++out_row_start;
            }
            ++lhs_row_start;
            ++rhs_row_start;
          }
        }
      }
    };

    template <typename Op, typename T1, typename T2, typename T3, typename I, typename J>
    void elementwise_binary_operation(Op &&op, const quantum_operator<T1, I, J> &lhs,
                                      const quantum_operator<T2, I, J> &rhs,
                                      quantum_operator<T3, I, J> &out) {
      assert(lhs.dim() == rhs.dim());
      assert(lhs.dim() == out.dim());

      WorkQueue(populate_row_tasks(op, lhs, rhs, out))
          .run(Schedule::SequentialBlocks, std::thread::hardware_concurrency());
    }
  }  // namespace details
}  // namespace quspin
