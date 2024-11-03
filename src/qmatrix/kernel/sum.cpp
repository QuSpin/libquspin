#include <cassert>
#include <quspin/array/array.hpp>
#include <quspin/details/dispatch.hpp>
#include <quspin/details/select.hpp>
#include <quspin/qmatrix/kernel/details/sum.hpp>
#include <quspin/qmatrix/qmatrix.hpp>
#include <string>

namespace quspin {

  struct MismatchInputIndexTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError, "mismatch input index dtypes.");
    }
  };

  struct MismatchInputCoeffIndexTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError,
                            "mismatch input coefficient index dtypes.");
    }
  };

  struct MismatchInputDataTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError, "mismatch in input data dtypes.");
    }
  };

  struct MismatchOutputIndexTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError, "mismatch in output index dtypes.");
    }
  };

  struct MismatchOutputCoeffIndexTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError,
                            "mismatch output coefficient index dtypes.");
    }
  };

  struct MismatchOutputDataTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError, "mismatch in output data dtype.");
    }
  };

  QuantumOperator sum(const QuantumOperator lhs, const QuantumOperator rhs,
                      const std::size_t num_threads /* = 0 */) {
    using namespace details;

    auto static_check_inputs = [](const auto &lhs, const auto &rhs) {
      using lhs_index_t = typename std::decay_t<decltype(lhs)>::index_type;
      using rhs_index_t = typename std::decay_t<decltype(rhs)>::index_type;

      using lhs_cindex_t = typename std::decay_t<decltype(lhs)>::cindex_type;
      using rhs_cindex_t = typename std::decay_t<decltype(rhs)>::cindex_type;

      using lhs_data_t = typename std::decay_t<decltype(lhs)>::value_type;
      using rhs_data_t = typename std::decay_t<decltype(rhs)>::value_type;

      if constexpr (!std::is_same_v<lhs_index_t, rhs_index_t>) {
        return MismatchInputIndexTypes();
      } else if constexpr (!std::is_same_v<lhs_cindex_t, rhs_cindex_t>) {
        return MismatchInputCoeffIndexTypes();
      } else if constexpr (!std::is_same_v<lhs_data_t, rhs_data_t>) {
        return MismatchInputDataTypes();
      } else {
        return details::ValidArgs();
      }
    };

    auto static_checks_nonzero
        = [&static_check_inputs](const auto &lhs, const auto &rhs, auto &out) {
            using lhs_index_t = typename std::decay_t<decltype(lhs)>::index_type;
            using out_t = typename std::decay_t<decltype(out)>::value_type;

            if constexpr (!std::is_same_v<lhs_index_t, out_t>) {
              return MismatchOutputIndexTypes();
            } else {
              return static_check_inputs(lhs, rhs);
            }
          };

    auto dynamic_checks_nonzero = [](const auto &lhs, const auto &rhs, auto &out) {
      if (lhs.dim() != rhs.dim() || lhs.indptr().shape(0) != out.shape(0)) {
        return details::ReturnVoidError(
            details::Error(details::ErrorType::ValueError, "Incompatible shapes"));
      }
      return details::ReturnVoidError();
    };

    Array indptr = lhs.indptr().copy();

    auto out = details::select<array<int32_t>, array<int64_t>>(indptr);

    auto op = [](auto &&lhs, auto &&rhs) {
      using lhs_t = std::decay_t<decltype(lhs)>;
      using rhs_t = std::decay_t<decltype(rhs)>;
      static_assert(std::is_same_v<lhs_t, rhs_t>, "Mismatching types");
      return static_cast<lhs_t>(lhs + rhs);
    };

    auto calc_nonzeros = [&op, &num_threads](const auto &lhs, const auto &rhs, auto &out) {
      elementwise_binop_size(op, lhs, rhs, out, num_threads);
      return details::ReturnVoidError();
    };

    details::dispatch(calc_nonzeros, static_checks_nonzero, dynamic_checks_nonzero, lhs, rhs, out);

    auto static_checks_operator
        = [&static_check_inputs](const auto &lhs, const auto &rhs, auto &res) {
            using lhs_index_t = typename std::decay_t<decltype(lhs)>::index_type;
            using res_index_t = typename std::decay_t<decltype(res)>::index_type;

            using lhs_cindex_t = typename std::decay_t<decltype(lhs)>::cindex_type;
            using res_cindex_t = typename std::decay_t<decltype(res)>::cindex_type;

            using lhs_data_t = typename std::decay_t<decltype(lhs)>::value_type;
            using res_data_t = typename std::decay_t<decltype(res)>::value_type;

            if constexpr (!std::is_same_v<lhs_index_t, res_index_t>) {
              return MismatchOutputIndexTypes();
            } else if constexpr (!std::is_same_v<lhs_cindex_t, res_cindex_t>) {
              return MismatchOutputCoeffIndexTypes();
            } else if constexpr (!std::is_same_v<lhs_data_t, res_data_t>) {
              return MismatchOutputDataTypes();
            } else {
              return static_check_inputs(lhs, rhs);
            }
          };

    auto dynamic_checks_operator = [](const auto &lhs, const auto &rhs, auto &res) {
      if (lhs.dim() != rhs.dim() || lhs.dim() != res.dim()) {
        return details::ReturnVoidError(
            details::Error(details::ErrorType::ValueError, "Incompatible shapes"));
      }
      return details::ReturnVoidError();
    };

    auto calc_qmatrix = [&op, &num_threads](const auto &lhs, const auto &rhs, auto &result) {
      elementwise_binary_operation(op, lhs, rhs, result, num_threads);
      return details::ReturnVoidError();
    };

    const std::size_t nnz = indptr[{indptr.size() - 1}];
    Array indices({nnz}, lhs.indices().dtype());
    Array cindices({nnz}, lhs.cindices().dtype());
    Array data({nnz}, lhs.dtype());

    QuantumOperator result(data, indptr, indices, cindices);

    details::dispatch(calc_qmatrix, static_checks_operator, dynamic_checks_operator, lhs, rhs,
                      result);

    return result;
  }
}  // namespace quspin
