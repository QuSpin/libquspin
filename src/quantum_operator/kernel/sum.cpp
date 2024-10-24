
#include <cassert>
#include <quspin/array/array.hpp>
#include <quspin/details/dispatch.hpp>
#include <quspin/details/select.hpp>
#include <quspin/quantum_operator/kernel/details/sum.hpp>
#include <quspin/quantum_operator/quantum_operator.hpp>
#include <string>

namespace quspin {

  struct MismatchIndexTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError, "mismatch in index dtypes.");
    }
  };

  struct MismatchCoeffIndexTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError,
                            "mismatch in coefficient index dtypes.");
    }
  };

  struct MismatchOutIndexTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError, "mismatch in output index dtypes.");
    }
  };

  struct MismatchOutDataTypes {
    static constexpr bool has_error() { return true; }
    static details::Error get_error() {
      return details::Error(details::ErrorType::ValueError, "mismatch in output data dtypes.");
    }
  };

  template <typename Op>
  QuantumOperator sum(Op &&op, const QuantumOperator lhs, const QuantumOperator rhs) {
    using namespace details;

    auto static_checks_nonzero = [](const auto &lhs, const auto &rhs, auto &out) {
      using lhs_index_t = typename std::decay_t<decltype(lhs)>::index_type;
      using rhs_index_t = typename std::decay_t<decltype(rhs)>::index_type;
      using lhs_cindex_t = typename std::decay_t<decltype(lhs)>::cindex_type;
      using rhs_cindex_t = typename std::decay_t<decltype(rhs)>::cindex_type;
      using out_t = typename std::decay_t<decltype(out)>::value_type;

      if constexpr (!std::is_same_v<lhs_index_t, rhs_index_t>) {
        return MismatchIndexTypes();
      } else if constexpr (!std::is_same_v<lhs_cindex_t, rhs_cindex_t>) {
        return MismatchCoeffIndexTypes();
      } else if constexpr (!std::is_same_v<lhs_index_t, out_t>) {
        return MismatchOutIndexTypes();
      } else {
        return details::ValidArgs();
      }
    };

    auto dynamic_checks_nonzero = [](const auto &lhs, const auto &rhs, auto &out) {
      if (lhs.dim() != rhs.dim() || lhs.indptr().shape() != out.shape(0)) {
        return details::ReturnVoidError(
            details::Error(details::ErrorType::ValueError, "Incompatible shapes"));
      }
      return details::ReturnVoidError();
    };

    Array indptr = lhs.indptr().copy();

    auto out = details::select<array<int32_t>, array<int64_t>>(indptr);

    auto calc_nonzeros
        = [&op](const auto &lhs, const auto &rhs, auto &out) { get_sum_size(op, lhs, rhs, out); };

    details::dispatch(calc_nonzeros, static_checks_nonzero, dynamic_checks_nonzero, lhs, rhs, out);

    auto static_checks_operator = [](const auto &lhs, const auto &rhs, auto &res) {
      using lhs_index_t = typename std::decay_t<decltype(lhs)>::index_type;
      using rhs_index_t = typename std::decay_t<decltype(rhs)>::index_type;
      using res_index_t = typename std::decay_t<decltype(res)>::index_type;

      using lhs_cindex_t = typename std::decay_t<decltype(lhs)>::cindex_type;
      using rhs_cindex_t = typename std::decay_t<decltype(rhs)>::cindex_type;
      using res_cindex_t = typename std::decay_t<decltype(res)>::cindex_type;

      using lhs_data_t = typename std::decay_t<decltype(lhs)>::value_type;
      using rhs_data_t = typename std::decay_t<decltype(rhs)>::value_type;
      using res_data_t = typename std::decay_t<decltype(res)>::value_type;

      using expected_data_t = details::upcast_t<lhs_data_t, rhs_data_t>;

      if constexpr (!std::is_same_v<lhs_index_t, res_index_t>
                    | !std::is_same_v<rhs_index_t, res_index_t>) {
        return MismatchIndexTypes();
      } else if constexpr (!std::is_same_v<lhs_cindex_t, res_cindex_t>
                           | !std::is_same_v<rhs_cindex_t, res_cindex_t>) {
        return MismatchCoeffIndexTypes();
      } else if constexpr (!std::is_same_v<expected_data_t, res_data_t>) {
        return MismatchOutDataTypes();
      } else {
        return details::ValidArgs();
      }
    };

    auto dynamic_checks_operator = [](const auto &lhs, const auto &rhs, auto &res) {
      if (lhs.dim() != rhs.dim() || lhs.dim() != res.dim()) {
        return details::ReturnVoidError(
            details::Error(details::ErrorType::ValueError, "Incompatible shapes"));
      }
      return details::ReturnVoidError();
    };

    auto calc_quantum_operator
        = [&op](const auto &lhs, const auto &rhs, auto &result) { sum_data(op, lhs, rhs, result); };

    const std::size_t nnz = indptr[{indptr.size()}];
    Array indices({nnz}, lhs.indices().dtype());
    Array cindices({nnz}, lhs.cindices().dtype());

    std::vector<DType> dtypes = {lhs.dtype(), rhs.dtype()};
    Array data({nnz}, result_dtype(dtypes));

    QuantumOperator result(data, indptr, indices, cindices);

    details::dispatch(calc_quantum_operator, static_checks_operator, dynamic_checks_operator, lhs,
                      rhs, result);

    return result;
  }

}  // namespace quspin
